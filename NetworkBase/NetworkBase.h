#pragma once

#include <WinSock2.h>
#pragma comment( lib, "wsock32.lib" )

#include <chrono>
#include <vector>
#include <mutex>
#include <iostream>

#define MAX_PACKET_SIZE 1300

class Address
{
private:
	unsigned int address = 0;
	unsigned short port = 0;
public:
	Address();
	Address(sockaddr_in addr);
	Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);
	Address(unsigned int address, unsigned short port);
	unsigned int GetAddress() const;
	sockaddr_in GetSockaddr() const;
	unsigned char GetA() const;
	unsigned char GetB() const;
	unsigned char GetC() const;
	unsigned char GetD() const;
	unsigned short GetPort() const;
	bool operator == (const Address& other) const;
	bool operator != (const Address& other) const;
};

bool SendUDPPacket(SOCKET sock, const char* packet, int packetSize, sockaddr_in address);

struct Packet
{
	Address from;
	int size = 0;
	char* data = nullptr;
	clock_t arriveTime;
	~Packet();
};

struct Connection
{
	bool active;
	Address address;
	unsigned long long connectionUId = 0;
	clock_t lastPingPacketTime = 0;
	std::vector<Packet> packets;

	std::mutex lock;
};

constexpr int PROTOCOLID = 139012;

/* standard offsets in every packet to send and receive */
enum PacketOffsets
{
	PROTOCOLIDOFFSET = 0,
	PACKETSIZEOFFSET = 4, // packetsize-4byte unsigned int
	PACKETIDOFFSET = 8, // packetid-4byte unsigned char
	CONNECTIONUIDOFFSET = 9,// connectionuid-8byte unsigned int(unsigned long long)
	PACKETNUMBEROFFSET = 17,// packetnumber-4byte unsigned int
	DATAOFFSET = 21
};

/* standard ids, that define how to handle packet */
enum PacketsIDs
{
	NEWCONNECTION = 0x01,
	PING = 0x02,
	GAMEDATA = 0x03,
	DISCONNECT = 0xff
};

enum PlayerAccessLevels
{
	NONE,
	DEFAULT,
	MEDIUM,
	HIGH,
	GOD
};

enum GameRoomStates
{
	game,
	pauseGame,
	endOfGame,
	lobby
};

struct ComperessedSnake
{
	unsigned int bodyOffset = 0; // SnakeBlocks of this snake offset from snakesBodiesOffset
	short bodySize = 0;
};

struct GameData
{
	unsigned int snakesOffset; // ComperessedSnakes offset in bytes from the beginning of game data in packet
	short numberOfSnakes;
	unsigned int snakesBodiesOffset; // SnakeBlocks offset in bytes from the beginning of game data in packet
};