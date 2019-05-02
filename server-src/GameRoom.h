#pragma once
#include "GamePlayEngine_server.h"
#include "NetworkManager.h"

struct Player
{
	unsigned long long connectionUID;
	Snake* snake;
};

enum GameRoomStates
{
	game,
	pauseGame,
	endOfGame,
	lobby
};

class GameRoom
{
private:
	NetworkManager* net;
	GamePlayEngine_server gamePlayEngine;
	std::vector<Player> players;
	GameRoomStates state;
	void MovePlayers();
	void HandlePlayersInput();
	void StartGame();
	void PauseGame();
	void EndGame();
	void SendGameDataToPlayers();

public:
	void Tick();
};