#pragma once
#include <mutex>

#include "AsyncArray.h"
#include "NetworkBase.h"

/* May be big object, don't create on stack */
class NetworkManager
{
private:
	std::mutex lock;

	SOCKET sock = 0;
	std::vector<Packet> incomingConnections;
	int numberOfActiveConnections = 0;
	static const int maxNumberOfConnections = 1000;
	Connection connections[maxNumberOfConnections];

	unsigned long long newConnectionUId = 0;

	public:static const int maxPacketSize = 10000;
private:
	char newPacketBuffer[maxPacketSize];

	bool networkManagerWorking = true;

	bool SendPacket(const char* packet, int packet_size, sockaddr_in address);
public:
	NetworkManager(unsigned short port);
	~NetworkManager();
	bool SendPacket(const char* packet, int packet_size, unsigned long long connectionUId);
	bool RecvPacket();
	bool AcceptConnection();
	void Disconnect(unsigned long long connectionUId);
	Connection* GetConnectionByUId(unsigned long long connectionUId);
};