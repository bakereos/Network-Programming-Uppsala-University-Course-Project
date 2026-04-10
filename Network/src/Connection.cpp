#include "Connection.h"
#include "raylibOverlay.h" // For raylibOverlay::GetTime() if still used for connectionInitTime
#include <iostream> // For debugging if needed

Connection::Connection() :
    state(ConnectionState::NoConnection),
    timeoutDurationSeconds(30.0f), // Default timeout
    totalByteSent(0),
    totalByteReceived(0),
    totalPacketSent(0),
    totalPacketReceived(0),
    totalMessageSent(0),
    totalMessageReceived(0),
    connectionInitTime(0.0),
    RTT(0.0f),
    canCalculateRTT(false),
    id(-1)
{
    InitTimers(); // Initialize time points
}

void Connection::InitTimers() {
    lastRecvTime = std::chrono::steady_clock::now();
    lastSendTime = std::chrono::steady_clock::now();
    connectionInitTime = raylibOverlay::GetTime(); // Using raylibOverlay for consistency with original, could be chrono too
}

void Connection::SetAddress(sockaddr_in addr)
{
    address = addr;
}

sockaddr_in& Connection::GetAddress()
{
    return address;
}

void Connection::SetConnectionState(ConnectionState state_) // Renamed from SetConnectionState
{
    this->state = state_;
    if (state_ == ConnectionState::Connected) {
        // Reset stats and timers when connection is (re)established
        ResetInfo();
    }
}

ConnectionState Connection::GetConnectionState() const 
{
    return state;
}

void Connection::UpdateLastRecvTime() // Renamed from UpdateLastRecvTime
{
    lastRecvTime = std::chrono::steady_clock::now();
}

void Connection::UpdateLastSendTime() // Renamed from UpdateLastSendTime
{
    lastSendTime = std::chrono::steady_clock::now();
}

void Connection::SetTimeoutDuration(float seconds) {
    timeoutDurationSeconds = seconds;
}

bool Connection::IsTimedOut() const // Renamed from isTimedout and added const
{
    if (state == NoConnection || state == Disconnected || state == Timedout) return false; // No timeout if not actively connected or trying

    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed_seconds = now - lastRecvTime;
    return elapsed_seconds.count() > timeoutDurationSeconds;
}

void Connection::IncreasePacketSent()
{
    totalPacketSent++;
}

void Connection::IncreasePacketReceived()
{
    totalPacketReceived++;
}

void Connection::AddAmountOfByteSent(int byteSent)
{
    totalByteSent += byteSent;
}

void Connection::AddAmountOfByteReceived(int byteReceived)
{
    totalByteReceived += byteReceived;
}

void Connection::AddAmountOfMessageSent(int messageSent)
{
    totalMessageSent += messageSent;
}

void Connection::AddAmountOfMessageReceived(int messageReceived)
{
    totalMessageReceived += messageReceived;
}

void Connection::CalculateRTT()
{
    if (!canCalculateRTT) return;
 
    std::chrono::duration<float> diff = lastRecvTime - lastSendTime; 
    RTT = std::abs(diff.count() * 1000.0f); 
    canCalculateRTT = false;
}

void Connection::SetCanCalculateRTT(bool canCalc) { 
    canCalculateRTT = canCalc;
}


float Connection::GetKbSentPerSecond() const 
{
    double timeElapsed = raylibOverlay::GetTime() - connectionInitTime;
    if (timeElapsed <= 0.0) return 0.0f;
    return float((totalByteSent / 1000.0f) / timeElapsed);
}

float Connection::GetKbReceivedPerSecond() const 
{
    double timeElapsed = raylibOverlay::GetTime() - connectionInitTime;
    if (timeElapsed <= 0.0) return 0.0f;
    return float((totalByteReceived / 1000.0f) / timeElapsed);
}

float Connection::GetPacketSentPerSecond() const 
{
    double timeElapsed = raylibOverlay::GetTime() - connectionInitTime;
    if (timeElapsed <= 0.0) return 0.0f;
    return float(totalPacketSent / timeElapsed);
}

float Connection::GetPacketReceivedPerSecond() const 
{
    double timeElapsed = raylibOverlay::GetTime() - connectionInitTime;
    if (timeElapsed <= 0.0) return 0.0f;
    return float(totalPacketReceived / timeElapsed);
}

float Connection::GetRTT() const 
{
    return RTT;
}

int Connection::GetTotalPacketSent() const { return totalPacketSent; } 
int Connection::GetTotalPacketReceived() const { return totalPacketReceived; } 
int Connection::GetTotalByteSent() const { return totalByteSent; } 
int Connection::GetTotalByteReceived() const { return totalByteReceived; } 
int Connection::GetTotalMessageSent() const { return totalMessageSent; } 
int Connection::GetTotalMessageReceived() const { return totalMessageReceived; } 

void Connection::ResetInfo()
{
    totalByteSent = 0;
    totalByteReceived = 0;
    totalPacketSent = 0;
    totalPacketReceived = 0;
    totalMessageSent = 0;
    totalMessageReceived = 0;
    connectionInitTime = raylibOverlay::GetTime(); 
    RTT = 0;
    InitTimers(); 
}

void Connection::SetID(int ID)
{
    this->id = ID;
}

int Connection::GetID() const 
{
    return id;
}

