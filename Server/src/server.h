#pragma once
#include "Network.h"
#include "Connection.h"
#include <vector>
#include "Entity.h"
#include "TimeSync.h"

class Server
{
	 Network network;
	 std::vector<Connection> connections;
	double lastSendTime, sendDelay = 0.100;
       public:
		   TimeSyncer timeSyncer;
		   bool Empty() { return connections.size() == 0; }
        float startTime;
		double baseTime;
	void Init();

	void CheckTimeout(DataBuffer& receiveBuffer);

	bool Transmit(const DataBuffer& dataBuffer);
	void Receive(DataBuffer& dataBuffer);

	void WriteConnectionPacket(byte_stream& stream);
	void HandleConnectionPacket(byte_stream_reader& reader, sockaddr_in& remote, DataBuffer& receiveBuffer);

	void WritePayloadPacket(byte_stream& stream, const DataBuffer& sendBuffer);
	void HandlePayloadPacket(byte_stream_reader& reader, sockaddr_in& remote, DataBuffer& receiveBuffer);

	void WriteDisconnectingPacket(byte_stream& stream);
	void HandleDisconnectingPacket(byte_stream_reader& reader, sockaddr_in& remote, DataBuffer& receiveBuffer);

	void ReplicateNewPlayerToClients(Player& newPlayer, std::vector<Entity*>& allEntities);
	void ReplicateNewEntitiesToClients(std::vector<Entity*>& newEntities);

	void DestroyReplicatedPlayer(Player* playerToDestroy);

	void SendTimeoutEvent(Connection& connection);
	void SendEndGameEvent(Player* player);
	void SendStartGameEvent();

	void DrawConnectionInfo(raylibOverlay::Vector2 position);

	void Clear();

private:
	Connection* FindConnection(sockaddr_in address);
	void RemoveConnection(sockaddr_in address);
};

