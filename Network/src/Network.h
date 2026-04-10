#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include "Serialization.h"
#define SERVER_PORT 55555

class Network
{
	WSADATA data;
	sockaddr_in local_addr;
	SOCKET sock;

public:
	//Initiation
	void InitWSA();

	//Socket functions
	void CreateSocket();
	void BindSocket(int family, u_short port_ = -1);
	void SetBroadcast(char enable);
	void SetNonBlocking(ULONG enable);

	//Data transfer
	void SendData(sockaddr_in& des, byte_stream& stream);
	bool RecvData(sockaddr_in& src, byte_stream& stream);

	void SetReUsable(int enable);

	//Cleanup
	void Clear();
};