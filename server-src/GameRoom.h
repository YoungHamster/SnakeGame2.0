#pragma once
#include "GamePlayEngine_server.h"
#include "NetworkManager.h"

struct Player
{
	unsigned long long connectionUID;
	PlayerAccessLevels accessLevel = DEFAULT;
	Snake* snake;
};

/* May be big object, don't create on stack */
class GameRoom
{
private:
	NetworkManager* net;
	GamePlayEngine_server gamePlayEngine;
	std::vector<Player> players;
	GameRoomStates state = lobby;
	clock_t lastGameTickTime = 0;
	int gameTickPeriod = 100;

	char sendPacketBuffer[MAX_PACKET_SIZE];

	void ProcessGameDataPacket(char* packet, int packetSize);
	void ProcessPlayersInput();
	void StartGame();
	void PauseGame();
	void EndGame();
	void SendGameDataToPlayers();

public:
	GameRoom(NetworkManager* net, int gameFieldWidth, int gameFieldHeight);
	~GameRoom();
	void Tick();
};