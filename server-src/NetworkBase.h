#pragma once

#include <WinSock2.h>
#pragma comment( lib, "wsock32.lib" )

#include <chrono>
#include <vector>

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
	Address address;
	unsigned long long connectionUId = 0;
	clock_t lastPingPacketTime = 0;
	std::vector<Packet> packets;
};

constexpr int PROTOCOLID = 139012;

/* standard offsets in every packet to send and receive */
enum PacketOffsets
{
	PROTOCOLIDOFFSET = 0,
	PACKETIDOFFSET = 4,
	CONNECTIONUIDOFFSET = 5,
	PACKETNUMBEROFFSET = 13,
	DATAOFFSET = 17
};

/* standard ids, that define reaction on packet */
enum PacketsIDs
{
	NEWCONNECTION = 0x01,
	PING = 0x02,
	GAMEDATA = 0x03,
	DISCONNECT = 0xff
};