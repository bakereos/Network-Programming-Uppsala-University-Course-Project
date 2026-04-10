#include "Network.h"


void Network::InitWSA()
{
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		std::cout << "ERROR: WSA STARTUP" << std::endl;

		return;
	}

	std::cout << "WSA STARTUP SUCCESS" << std::endl;
}

void Network::CreateSocket()
{
	sock = socket(AF_INET, SOCK_DGRAM, 0);

	if (sock != INVALID_SOCKET)
	{
		std::cout << "CREATE SOCKET SUCCESS" << std::endl;
		return;
	}

	std::cout << "ERROR: CREATE SOCKET" << std::endl;
}

void Network::BindSocket(int family, u_short port_)
{
	local_addr.sin_family = (ADDRESS_FAMILY)family;

	if (port_ > -1)
	{
		local_addr.sin_port = htons(port_);
	}

	local_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (::bind(sock, (const sockaddr*)&local_addr, INET_ADDRSTRLEN) == SOCKET_ERROR)
	{
		std::cout << "ERROR: BINDING SOCKET" << std::endl;
	}

	std::cout << "BIND SOCKET SUCCESS" << std::endl;
}

void Network::SetBroadcast(char enable)
{
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable)) != 0)
	{
		std::cout << "ERROR: ENABLE BROADCAST" << std::endl;
	}

	std::cout << "SET BROADCAST SUCCESS" << std::endl;
}

void Network::SetNonBlocking(ULONG enable)
{
	if (ioctlsocket(sock, FIONBIO, &enable) == SOCKET_ERROR)
	{
		std::cout << "ERROR: ENABLING NON-BLOCKING FAILED" << std::endl;
		return;
	}

	std::cout << "SET NON_BLOCKING SUCCESS" << std::endl;
}

void Network::SendData(sockaddr_in& des, byte_stream& stream)
{
	if (sendto(sock, stream.m_buffer, stream.m_size, 0, (const sockaddr*)&des, INET_ADDRSTRLEN) == SOCKET_ERROR)
	{
		std::cout << "ERROR: SEND DATA - " << WSAGetLastError() << std::endl;
	}
}

bool Network::RecvData(sockaddr_in& src, byte_stream& stream)
{
	int addrlen = INET_ADDRSTRLEN;

	int result = recvfrom(sock, stream.m_buffer, sizeof(stream.m_buffer), 0, (sockaddr*)&src, &addrlen);
	if (result == SOCKET_ERROR)
	{
		if (WSAGetLastError() != 10035)
		{
			std::cout << "ERROR: RECEIVE FAILED - " << WSAGetLastError() << std::endl;
		}

		return false;
	}

	stream.m_size = result;

	return true;
}

void Network::SetReUsable(int opt)
{
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
}

void Network::Clear()
{
	::closesocket(sock);
	sock = INVALID_SOCKET;
	WSACleanup();
}
