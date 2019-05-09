#pragma once
#include "GamePlayEngine_server.h"
#include "NetworkManager.h"

enum PlayerAccessLevels
{
	NONE,
	DEFAULT,
	MEDIUM,
	HIGH,
	GOD
};

struct Player
{
	unsigned long long connectionUID;
	PlayerAccessLevels accessLevel = DEFAULT;
	Snake* snake;
};

enum GameRoomStates
{
	game,
	pauseGame,
	endOfGame,
	lobby
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

	char newPacketBuffer[NetworkManager::maxPacketSize];

	void HandlePlayersInput();
	void StartGame();
	void PauseGame();
	void EndGame();
	void SendGameDataToPlayers();

public:
	GameRoom(NetworkManager* net, int physw, int physh);
	~GameRoom();
	void Tick();
};