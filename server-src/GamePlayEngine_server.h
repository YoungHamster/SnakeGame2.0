#pragma once
#include <vector>

#include "src//GamePlayStructsAndDefines.h"
#include "src//SnakesManager.h"

struct ComperessedSnake
{
	unsigned int bodyOffset = 0; // SnakeBlocks of this snake offset from snakesBodiesOffset
	short bodySize = 0;
};

struct GameData
{
	unsigned int snakesOffset; // ComperessedSnakes offset in bytes from the beginning of game data in packet
	short numberOfSnakes;
	unsigned int snakesBodiesOffset; // SnakeBlocks offset in bytes from the beginning of game data in packet
};

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

	void GameTick();

	char* GetGameData(int* dataSize);
	int GetNumberOfSnakes();
	int GetNumberOfSnakeBlocks();
};