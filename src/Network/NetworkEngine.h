#pragma once
#include "NetworkBase//NetworkBase.h"

struct NetworkInput
{

};

class NetworkEngine
{
private:
	SOCKET sock;
	Address serverAddress;

	char sendPacketBuffer[MAX_PACKET_SIZE];
	char recvPacketBuffer[MAX_PACKET_SIZE];
	std::vector<Packet> receivedPackets;

	bool SendPacket();
	bool RecvPacket();
	bool CollectGameDataFromPackets();
public:
	NetworkEngine();
	~NetworkEngine();
	bool Connect(Address address);
	void Disconnect();
	void GameTick();
	void SendInput(NetworkInput input);
};