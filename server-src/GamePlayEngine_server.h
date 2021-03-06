#pragma once
#include <vector>

#include "src//GamePlayStructsAndDefines.h"
#include "src//SnakesManager.h"
#include "NetworkBase//NetworkBase.h"

#define DEFAULT_SNAKE_SIZE 3

class GamePlayEngine_server
{
private:
	int physw = 0;
	int physh = 0;
	PhysicalObject* physics;
	int* number_of_snakes;
	SnakesManager snakesManager;

	AIWeight foodWeights[SNAKE_FOV_WIDTH][SNAKE_FOV_HEIGHT] = {
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, 0, 1, 0),   AIWeight(1, 0, 1, 0),   AIWeight(1, 0, 1, 0),   AIWeight(1, -1, 0, 0), AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1) },
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, 0, 1, 0),   AIWeight(1, 0, 1, 0),   AIWeight(1, -1, 1, -1), AIWeight(2, -2, 0, 0), AIWeight(1, -1, -1, 1), AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1) },
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, 0, 1, 0),   AIWeight(1, -1, 1, -1), AIWeight(2, -2, 2, -2), AIWeight(3, -3, 0, 0), AIWeight(2, -2, -2, 2), AIWeight(1, -1, -1, 1), AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1) },
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, -1, 1, -1), AIWeight(2, -2, 2, -2), AIWeight(3, -3, 3, -3), AIWeight(4, -4, 0, 0), AIWeight(3, -3, -3, 3), AIWeight(2, -2, -2, 2), AIWeight(1, -1, -1, 1), AIWeight(1, 0, 0, 1) },
	{ AIWeight(0, 0, 1, -1), AIWeight(0, 0, 2, -2),  AIWeight(0, 0, 3, -3),  AIWeight(0, 0, 4, -4),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -4, 4),  AIWeight(0, 0, -3, 3),  AIWeight(0, 0, -2, 2),  AIWeight(0, 0, -1, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(-1, 1, 1, -1), AIWeight(-2, 2, 2, -2), AIWeight(-3, 3, 3, -3), AIWeight(-4, 4, 0, 0), AIWeight(-3, 3, -3, 3), AIWeight(-2, 2, -2, 2), AIWeight(-1, 1, -1, 1), AIWeight(0, 1, 0, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(0, 1, 1, 0),   AIWeight(-1, 1, 1, -1), AIWeight(-2, 2, 2, -2), AIWeight(-3, 3, 0, 0), AIWeight(-2, 2, -2, 2), AIWeight(-1, 1, -1, 1), AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(0, 1, 1, 0),   AIWeight(0, 1, 1, 0),   AIWeight(-1, 1, 1, -1), AIWeight(-2, 2, 0, 0), AIWeight(-1, 1, -1, 1), AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(0, 1, 1, 0),   AIWeight(0, 1, 1, 0),   AIWeight(0, 1, 1, 0),   AIWeight(-1, 1, 0, 0), AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1) }
	};
	AIWeight obstacleWeights[SNAKE_FOV_WIDTH][SNAKE_FOV_HEIGHT] = {
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(-1, 1, 1, 1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -1, 0), AIWeight(-2, 2, 2, 2), AIWeight(0, 0, 0, -1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -2, 0), AIWeight(-3, 3, 3, 3), AIWeight(0, 0, 0, -2), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(3, 3, -3, 3), AIWeight(-4, 4, 4, 4), AIWeight(3, 3, 3, -3), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(1, 1, -1, 1), AIWeight(2, 2, -2, 2), AIWeight(3, 3, -3, 3), AIWeight(4, 4, -4, 4), AIWeight(0, 0, 0, 0),  AIWeight(4, 4, 4, -4), AIWeight(3, 3, 3, -3), AIWeight(2, 2, 2, -2), AIWeight(1, 1, 1, -1) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(3, 3, -3, 3), AIWeight(4, -4, 4, 4), AIWeight(3, 3, 3, -3), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -2, 0), AIWeight(3, -3, 3, 3), AIWeight(0, 0, 0, -2), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -1, 0), AIWeight(2, -2, 2, 2), AIWeight(0, 0, 0, -1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(1, -1, 1, 1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) }
	};

	int appleSpawnFrequency = 1;

	unsigned int tickNumber = 0;

	void ChangeObject(int x, int y, int type);
	int GetObjType(int x, int y);
	void DespawnSnake(int snake_id);
	void FeedSnake(int snake_id);
	void ShortenSnake(int snake_id);
	void HandleSnakeAI(int snake_id);
	void SplitSnake(int snake_id);

	int SpawnSnake(int size, int headx, int heady, char headdir, AITypes aiType);
	void SplitSnakes(int minLenghtToSplit);
	void SpawnApple();
	void MoveSnakes();

	bool IsAnyPlayerAlive();
	void KillAllSnakes();
public:
	GamePlayEngine_server(int physw, int physh);
	~GamePlayEngine_server();

	void SetAppleSpawnFrequency(int applesPerTick);
	void ChangeSnakeDirection(int snake_id, char dir);

	void StartGame(int numberOfSnakes, int snakesSizes);
	void GameTick();

	char* GetGameData(int* dataSize);
	int GetNumberOfSnakes();
	int GetNumberOfSnakeBlocks();
	int GetNumberOfApples();
	int GetPhysicsSize();
	const PhysicalObject* GetPhysics();
};