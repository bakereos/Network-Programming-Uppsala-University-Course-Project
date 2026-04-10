#pragma once
#include <WinSock2.h>
#include "Shared.h"
#include <chrono> 

class Connection
{
	int id = -1;
	sockaddr_in address;
	ConnectionState state = ConnectionState::NoConnection;

	std::chrono::steady_clock::time_point lastRecvTime; 
	std::chrono::steady_clock::time_point lastSendTime; 
	float timeoutDurationSeconds = 30.0f;
	int totalByteSent, totalByteReceived;
	int totalPacketSent, totalPacketReceived;
	int totalMessageSent, totalMessageReceived;
	double connectionInitTime; 
	float RTT;
	bool canCalculateRTT;

public:
	Connection();
	void SetAddress(sockaddr_in addr);
	sockaddr_in& GetAddress();

	void SetConnectionState(ConnectionState state_); 
	ConnectionState GetConnectionState() const; 

	void SetID(int ID);
	int GetID() const; 

	void UpdateLastRecvTime(); 
	void UpdateLastSendTime(); 
	bool IsTimedOut() const; 
	void SetTimeoutDuration(float seconds);

	void IncreasePacketSent();
	void IncreasePacketReceived();
	void AddAmountOfByteSent(int byteSent);
	void AddAmountOfByteReceived(int byteReceived);
	void AddAmountOfMessageSent(int messageSent);
	void AddAmountOfMessageReceived(int messageReceived);
	void CalculateRTT();
	void SetCanCalculateRTT(bool canCalc); 

	float GetKbSentPerSecond() const; 
	float GetKbReceivedPerSecond() const;
	float GetPacketSentPerSecond() const;
	float GetPacketReceivedPerSecond() const;
	float GetRTT() const; 
	int GetTotalPacketSent() const; 
	int GetTotalPacketReceived() const; 
	int GetTotalByteSent() const; 
	int GetTotalByteReceived() const; 
	int GetTotalMessageSent() const; 
	int GetTotalMessageReceived() const; 

	void ResetInfo();
	void InitTimers(); // To initialize time points
};
