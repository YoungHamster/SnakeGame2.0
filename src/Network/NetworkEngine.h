#pragma once
#include "NetworkBase//NetworkBase.h"

struct NetworkInput
{

};

const int MaxNumberOfAttemptsToConnect = 10;

class NetworkEngine
{
private:
	SOCKET sock;
	Address serverAddress;
	ConnectionStates connectionState = DISCONNECTED;

	char sendPacketBuffer[MAX_PACKET_SIZE];
	char recvPacketBuffer[MAX_PACKET_SIZE];
	std::vector<Packet> receivedPackets;

	unsigned long long connectionUId;
	clock_t lastPongPacketTime;

	unsigned int currentTickNumber = 0;
	bool collectingCurrentTickGameData = false;
	char* currentTickGameData = nullptr;

	bool SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId);
	bool RecvPacket();
	bool CollectGameDataFromPackets();
	void ProcessIncomingPacket();

	void StartGame();
public:
	NetworkEngine();
	~NetworkEngine();
	bool Connect(Address address);
	void Disconnect();
	void GameTick();
	void SendInput(NetworkInput input);
};