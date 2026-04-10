#include "Client.h"
#include <random>

void Client::Init()
{
    // Initialize network
    network.InitWSA();
    network.CreateSocket();

    // Bind to a dynamic port (to avoid conflicts)
    sockaddr_in localAddress;
    localAddress.sin_family = AF_INET;
    localAddress.sin_addr.s_addr = INADDR_ANY;
    localAddress.sin_port = 0; // Let OS choose a free port
    network.BindSocket(localAddress.sin_family, localAddress.sin_port);

    // Set non-blocking mode and enable broadcast
    network.SetNonBlocking(1);
    network.SetBroadcast(1);

    // Set up broadcast address for discovery phase
    broadcast.sin_family = AF_INET;
    broadcast.sin_port = htons(SERVER_PORT);
    broadcast.sin_addr.S_un.S_addr = INADDR_BROADCAST;
    connection.SetAddress(broadcast);

}

void Client::InitConnection()
{
    std::cout << "Initializing connection..." << std::endl;
    if (connection.GetConnectionState() == Connected)
    {
        return;
    }
    connection.ResetInfo();
    connection.SetConnectionState(Connecting);
}

void Client::InitDisconnection()
{
    if (connection.GetConnectionState() == Disconnecting || connection.GetConnectionState() == Disconnected)
    {
        return;
    }

    connection.SetConnectionState(Disconnecting);
}

void Client::CheckTimeout(DataBuffer& receiveBuffer) {

    if (connection.IsTimedOut()) {
        if (connection.GetConnectionState() != Disconnected) {
            receiveBuffer.headers.push_back(
                DataHeader(DestroyPlayer, connection.GetID()));
            SendTimeoutEvent();
            RemoveConnection();
        }
    }

}

void Client::SendTimeoutEvent() {
    byte_stream stream;
    byte_stream_writer writer(stream);

    writer.serialize_enum(DisconnectingPacket);

    writer.serialize(timeSyncer.GetNow());

    writer.serialize_enum(Timedout);

    network.SendData(connection.GetAddress(), stream);
}

bool Client::Transmit(const DataBuffer& sendBuffer)
{

    if (connection.GetConnectionState() == NoConnection) return false;
    if (raylibOverlay::SecondsPassedSinceTime(lastSendTime) < sendDelay) return false;

    byte_stream stream;

    switch (connection.GetConnectionState())
    {
    case Connecting:
        WriteConnectionPacket(stream);
        break;
    case WaitPayload:
        WriteConnectionPacket(stream);
        break;
    case Connected:
        WritePayloadPacket(stream, sendBuffer);
        break;
    case Disconnecting:
        WriteDisconnectingPacket(stream);
        break;
    case Disconnected:
        WriteDisconnectingPacket(stream);
        break;
    }
    connection.AddAmountOfByteSent(stream.m_size);
    connection.IncreasePacketSent();
    connection.AddAmountOfMessageSent((int)sendBuffer.headers.size());
    connection.UpdateLastSendTime();
    network.SendData(connection.GetAddress(), stream);

    if (connection.GetConnectionState() == Disconnected)
    {
        connection.SetConnectionState(NoConnection);
        connection.SetAddress(broadcast);
    }

    return true;
}

void Client::Receive(DataBuffer& receiveBuffer)
{
    byte_stream stream;
    sockaddr_in remote;
    if (!network.RecvData(remote, stream)) return;

    byte_stream_reader reader(stream);

    PacketType packetType;
    reader.serialize_enum(packetType);

    double sendClockTime;
    reader.serialize(sendClockTime);

    bool validPacket = true;

    switch (packetType)
    {
    case ConnectionPacket:
        HandleConnectionPacket(reader, remote);
        break;
    case PayloadPacket:
        HandlePayloadPacket(reader, receiveBuffer);
        break;
    case DisconnectingPacket:
        HandleDisconnectingPacket(reader, receiveBuffer);
        break;
    default:
        validPacket = false;
        break;
    }
    if (validPacket) {
        Connection* source = &connection;

        if (source == nullptr) return;

        source->UpdateLastRecvTime();
        source->SetCanCalculateRTT(true);
        source->CalculateRTT();
        source->UpdateLastRecvTime();
        source->AddAmountOfByteReceived(stream.m_size);
        source->IncreasePacketReceived();
        source->AddAmountOfMessageReceived((int)receiveBuffer.headers.size());
    }
}

void Client::WriteConnectionPacket(byte_stream& stream) {

    byte_stream_writer writer(stream);

    writer.serialize_enum(ConnectionPacket);
    writer.serialize(timeSyncer.GetNow());
    writer.serialize_enum(connection.GetConnectionState());
}

void Client::HandleConnectionPacket(byte_stream_reader& reader, sockaddr_in& remote)
{
    ConnectionState connectionState;
    reader.serialize_enum(connectionState);
    if (connectionState == Connecting)
    {
        connection.SetAddress(remote);
        connection.SetConnectionState(WaitPayload);
    }
}

void Client::WritePayloadPacket(byte_stream& stream, const DataBuffer& sendBuffer)
{
    byte_stream_writer writer(stream);

    std::vector<raylibOverlay::Vector2>::const_iterator vector2DataIter = sendBuffer.vector2Data.begin();
    std::vector<bool>::const_iterator boolDataIter = sendBuffer.boolData.begin();

    writer.serialize_enum(PayloadPacket);
    writer.serialize(timeSyncer.GetNow());


    for (const auto& header : sendBuffer.headers)
    {
        writer.serialize_enum(header.command);

        switch (header.command)
        {
        case MovementInput:
            writer.serialize_vector_2(*vector2DataIter);
            writer.serialize(*boolDataIter);

            vector2DataIter++;
            boolDataIter++;
        }
    }
}

void Client::HandlePayloadPacket(byte_stream_reader& reader, DataBuffer& receiveBuffer) {
    if (connection.GetConnectionState() == WaitPayload) {
        connection.SetConnectionState(Connected);
    }

    bool isFirstPacket = true;
    double adjustedTime = 0;
    while (true) {
        DataHeader receiveHeader;
        raylibOverlay::Vector2 receiveVector2;
        raylibOverlay::Color receiveColor;
        bool receiveBool;
        double receiveTime;

        if (!reader.serialize_enum(receiveHeader.command)) break;


        reader.serialize(receiveHeader.id);


        if (clientID == 0) {
            clientID = receiveHeader.id;
        }

        receiveBuffer.headers.push_back(receiveHeader);
        switch (receiveHeader.command) {
        case UpdatePosition:

            reader.serialize(receiveTime);
            reader.serialize_vector_2(receiveVector2);


            if (isFirstPacket) {
                timeSyncer.OnServerUpdate(receiveTime);
                isFirstPacket = false;
            }


            adjustedTime = receiveTime + GetTripTime();

            receiveBuffer.doubleData.push_back(adjustedTime);
            receiveBuffer.vector2Data.push_back(receiveVector2);
            break;

        case CreatePlayer:
            reader.serialize(receiveBool);
            reader.serialize_vector_2(receiveVector2);
            reader.serialize_color(receiveColor);

            receiveBuffer.boolData.push_back(receiveBool);
            receiveBuffer.vector2Data.push_back(receiveVector2);
            receiveBuffer.colorData.push_back(receiveColor);
            break;

        case CreateBomb:
            reader.serialize_vector_2(receiveVector2);
            reader.serialize_color(receiveColor);

            receiveBuffer.vector2Data.push_back(receiveVector2);
            receiveBuffer.colorData.push_back(receiveColor);
            break;

        case GameEnded:
            reader.serialize(receiveBool);
            receiveBuffer.boolData.push_back(receiveBool);
            break;
        }
    }
}



void Client::WriteDisconnectingPacket(byte_stream& stream) {
    byte_stream_writer writer(stream);

    writer.serialize_enum(DisconnectingPacket);
    writer.serialize(timeSyncer.GetNow());

    writer.serialize_enum(connection.GetConnectionState());
}

void Client::HandleDisconnectingPacket(byte_stream_reader& reader, DataBuffer& receiveBuffer) {
    ConnectionState connectionState;
    reader.serialize_enum(connectionState);
    if (connectionState == Disconnecting) {
        //Disconnection storage free-up
        receiveBuffer.headers.push_back(DataHeader(DestroyAllEntity, -1));
        connection.SetConnectionState(Disconnected);
    }

    if (connectionState == Timedout)
    {
        receiveBuffer.headers.push_back(DataHeader(DestroyAllEntity, -1));
        connection.SetAddress(broadcast);
        connection.SetConnectionState(Timedout);
    }
}

void Client::DrawConnectionInfo(raylibOverlay::Vector2 position) {
    if (connection.GetConnectionState() != Connected) {
        return;
    }

    std::string displayText = "";
    raylibOverlay::Color black = { 0, 0, 0, 255 };
    raylibOverlay::Vector2 textPos = position;
    int size = 20;
    displayText = "RTT(s): " + std::to_string(connection.GetRTT());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);
    textPos.y += 20;
    displayText = "Kb per second out: " +
        std::to_string(connection.GetKbSentPerSecond());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText =
        "Kb per second in: " +
        std::to_string(connection.GetKbReceivedPerSecond());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText =
        "Packet per second out: " +
        std::to_string(connection.GetPacketSentPerSecond());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText =
        "Packet per second in: " +
        std::to_string(connection.GetPacketReceivedPerSecond());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText = "Total byte sent: " +
        std::to_string(connection.GetTotalByteSent());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText = "Total byte received: " +
        std::to_string(connection.GetTotalByteReceived());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText = "Total packet sent: " +
        std::to_string(connection.GetTotalPacketSent());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText =
        "Total packet received: " +
        std::to_string(connection.GetTotalPacketReceived());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText = "Total message sent: " +
        std::to_string(connection.GetTotalMessageSent());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);

    textPos.y += 20;
    displayText =
        "Total message received: " +
        std::to_string(connection.GetTotalMessageReceived());
    raylibOverlay::DrawTextV(displayText.c_str(), textPos, size,
        black);
    textPos.y += 20;
    std::string temp =
        "Input Miscalculations: " +
        std::to_string(Client::inputMiscalculations);
    raylibOverlay::DrawTextV(temp.c_str(), textPos, 20, black);


    if (connection.GetConnectionState() == Timedout)
    {
        raylibOverlay::DrawTextV("YOU HAVE BEEN TIMED OUT BY THE SERVER", { 10, 50 }, 20, { 0, 0, 0, 255 });
    }
}


void Client::RemoveConnection() {
    connection.ResetInfo();
    connection.SetAddress(broadcast);
    connection.SetConnectionState(NoConnection);
}

float Client::GetTripTime() { return connection.GetRTT() / 2; }

void Client::Clear()
{
    network.Clear();
}
