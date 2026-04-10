#pragma once


#include "Network.h"
#include "Connection.h"
#include <string>
#include "TimeSync.h"
class Client
{
	Network network;
	sockaddr_in broadcast;
	double lastSendTime, sendDelay;

public:
	Connection connection;
	int clientID;
	double baseTime;
	TimeSyncer timeSyncer;
	static inline int inputMiscalculations = 0;
	void Init();

	void InitConnection();
	void InitDisconnection();

	bool Transmit(const DataBuffer& sendBuffer);
	void Receive(DataBuffer& receiveBuffer);

	void WriteConnectionPacket(byte_stream& stream);
	void HandleConnectionPacket(byte_stream_reader& reader, sockaddr_in& remote);

	void WritePayloadPacket(byte_stream& stream, const DataBuffer& sendBuffer);
	void HandlePayloadPacket(byte_stream_reader& reader, DataBuffer& receiveBuffer);

	void WriteDisconnectingPacket(byte_stream& stream);
	void HandleDisconnectingPacket(byte_stream_reader& stream, DataBuffer& receiveBuffer);

	void DrawConnectionInfo(raylibOverlay::Vector2 position);
	void CheckTimeout(DataBuffer& receiveBuffer);
	void SendTimeoutEvent();
	void RemoveConnection();
	float GetTripTime();

	void Clear();
};

