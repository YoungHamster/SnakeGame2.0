#pragma once
#include <mutex>

#include "AsyncArray.h"
#include "NetworkBase.h"

class NetworkManager
{
private:
	std::mutex lock;

	SOCKET sock = 0;
	std::vector<Packet> incomingConnections;
	Connection* connections;
	int numberOfActiveConnections = 0;
	int maxNumberOfConnections = 1000;

	unsigned long long newConnectionUId = 0;

	const int maxPacketSize = 10000;
	char* newPacketBuffer = nullptr;

	bool networkManagerWorking = true;

	bool SendPacket(const char* packet, int packet_size, sockaddr_in address);
	Connection GetConnectionByUId(unsigned long long connectionUId);
public:
	NetworkManager(unsigned short port);
	~NetworkManager();
	bool SendPacket(const char* packet, int packet_size, unsigned long long connectionUId);
	bool RecvPacket();
	bool AcceptConnection();
	void Disconnect(unsigned long long connectionUId);
};