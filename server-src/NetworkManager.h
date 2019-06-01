#pragma once

#include "NetworkBase//NetworkBase.h"

/* May be big object, don't create on stack */
class NetworkManager
{
private:
	SOCKET sock = 0;

	std::mutex incomingConnectionsLock;
	std::vector<Packet> incomingConnections;

	int numberOfActiveConnections = 0;
	static const int maxNumberOfConnections = 1000;
	Connection connections[maxNumberOfConnections];

	// 0-reserved connectionUId for inactive connections
	unsigned long long newConnectionUId = 1;

	char recvPacketBuffer[MAX_PACKET_SIZE];

	bool networkManagerWorking = true;

	bool SendPacket(const char* packet, int packetSize, sockaddr_in address);
public:
	NetworkManager(unsigned short port);
	~NetworkManager();
	bool SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId);
	bool SendPacketToAll(const char* packet, unsigned int packetSize, unsigned char packetId);
	bool SendPacketToListOfConnection(const char* packet, unsigned int packetSize, unsigned char packetId, unsigned long long* connectionsUIds, int numberOfConnectionsUIds);
	bool ReliablySendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId);
	/* Returns array of UIds of connections that didn't receive packet by any reason */
	unsigned long long* ReliablySendPacketToListOfConnections(const char* packet, unsigned int packetSize, unsigned char packetId, 
															 unsigned long long* connectionsUIds, int numberOfConnectionsUIds, 
															 int* numberOfConnectionsThatDidntReceivePacket);
	bool RecvPacket();
	bool AcceptConnection();
	void Disconnect(unsigned long long connectionUId);
	Connection* GetConnectionByUId(unsigned long long connectionUId);
};

struct ConnInfoForRDT // RDT-Reliable Data Transfer
{
	bool failed = false;
	bool succeed = false;
	bool sentPacket = false;
	clock_t packetSentTime = 0;
	unsigned int numberOfTries = 0;
};