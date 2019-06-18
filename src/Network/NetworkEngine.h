#pragma once
#include "NetworkBase//NetworkBase.h"
#include "GamePlayStructsAndDefines.h"

struct NetworkInput
{

};

struct GamePlayData
{
	PhysicalObject* physics;
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

	unsigned long long connectionUId = 0;
	clock_t lastPongPacketTime;

	unsigned int currentTickNumber = 0;
	bool collectingCurrentTickGameData = false;
	char* currentTickGameData = nullptr;
	char* prevTickGameData = nullptr;
	GamePlayData gamePlayData;

	bool SendPacket(const char* packet, unsigned int packetSize, unsigned long long connectionUId, unsigned char packetId);
	bool ReliablySendPacket(const char* packet, unsigned int packetSize, unsigned char packetId);
	bool RecvPacket();
	bool CollectGameDataFromPackets();
	void ProcessIncomingPacket();

	bool StartGame();
public:
	NetworkEngine();
	~NetworkEngine();
	bool Connect(Address address);
	void Disconnect();
	void GameTick();
	void SendInput(NetworkInput input);
};