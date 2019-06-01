#pragma once

#include <WinSock2.h>
#pragma comment( lib, "wsock32.lib" )

#include <chrono>
#include <vector>
#include <mutex>
#include <iostream>

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
	bool active = false;
	Address address;
	unsigned long long connectionUId = 0;
	clock_t lastPingPacketTime = 0;
	std::vector<Packet> packets;

	std::mutex lock;
};

enum ConnectionStates
{
	CONNECTED,
	DISCONNECTED
};

constexpr int32_t UNSAFEPROTOCOLID = 139012;
constexpr int32_t SAFEPROTOCOLID = 139013;

/* standard offsets in every packet to send and receive */
enum PacketOffsets
{
	PROTOCOLIDOFFSET = 0,
	PACKETSIZEOFFSET = 4, // packetsize-4byte unsigned int
	PACKETIDOFFSET = 8, // packetid-1byte unsigned char
	CONNECTIONUIDOFFSET = 9,// connectionuid-8byte unsigned int(unsigned long long)
	DATAOFFSET = 17
};

struct PacketHeader
{
	int32_t protocolId;
	uint32_t packetSize;
	uint8_t packetId;
	uint64_t connectionUId;
};

/* standard ids, that define how to handle packet */
enum PacketsIDs
{
	NEWCONNECTION = 0x01,
	PING = 0x02,
	PONG = 0x03,
	GAMEDATA = 0x04,
	NOCONF = 0xfd,
	CONF = 0xfe, // Confirmation that packet was received(used to reliably transfer data through UDP connections)
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

struct CompressedSnake
{
	unsigned int bodyOffset = 0; // SnakeBlocks of this snake offset from start of snakeblocks data
	short bodySize = 0;
};

struct AppleData
{
	short x;
	short y;
};

/* Structure of game data in byte array: */
/*  16bytes											GameDataHeader, 
	sizeof(CompressedSnake)*numberOfSnakes bytes	snakes info, 
	all the rest									SnakeBlocks*/
struct GameDataHeader
{
	uint32_t tickNumber;
	uint32_t sizeOfGameData;
	int16_t numberOfSnakes;
	uint16_t snakesOffset;
	uint32_t snakesBlocksOffset;
	uint32_t applesOffset;
};

// Data necessary to identify packet with game data
struct GameDataPacketHeader
{
	uint8_t packetNumber;
	uint32_t tickNumber;
};

#define SUPPOSED_MTU 1350
#define MAX_PACKET_SIZE SUPPOSED_MTU
#define FREE_PLACE_IN_SINGLE_GAMEDATA_PACKET SUPPOSED_MTU - DATAOFFSET - sizeof(GameDataPacketHeader)

#define NUMBER_OF_TRIES_IN_RELIABLE_TRANSFER_BEFORE_FAILURE 20 // failed to transfer data N times-stop trying
#define SINGLE_TRY_IN_RELIABLE_TRANSFER_DELAY 500 // in milliseconds