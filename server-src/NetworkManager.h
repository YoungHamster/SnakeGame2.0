#pragma once

#include "AsyncArray.h"
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
	bool SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId, unsigned int packetNumber);
	bool RecvPacket();
	bool AcceptConnection();
	void Disconnect(unsigned long long connectionUId);
	Connection* GetConnectionByUId(unsigned long long connectionUId);
};