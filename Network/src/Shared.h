#pragma once
#include <vector>
#include "raylibOverlay.h"


enum PacketType
{
	ConnectionPacket,
	PayloadPacket,
	DisconnectingPacket
};

enum ConnectionState
{
	NoConnection = -1,
	Connecting,
	WaitPayload,
	Connected,
	Disconnecting,
	Disconnected,
	Timedout
};

enum Command
{
	Undefined = -1,
	CreatePlayer,
	CreateBomb,
	UpdatePosition,
	MovementInput,
	DestroyBomb,
	DestroyPlayer,
	DestroyAllEntity,
	GameStarted,
	GameEnded,
	BoostInput
};

struct DataHeader
{
	Command command;
	int id;

	DataHeader()
	{
		command = Undefined;
		id = -1;
	}

	DataHeader(Command command, int id)
	{
		this->command = command;
		this->id = id;
	}
};

struct DataBuffer
{
	std::vector<DataHeader> headers;
	std::vector<raylibOverlay::Vector2> vector2Data;
	std::vector<raylibOverlay::Color> colorData;
	std::vector<bool> boolData;
	std::vector<double> doubleData;

	void ClearBuffer()
	{
		headers.clear();
		vector2Data.clear();
		colorData.clear();
		boolData.clear();
		doubleData.clear();
	}
};
