#pragma once

#include "GamePlayStructsAndDefines.h"

class SnakesManager
{
private:
	SnakeBlock* snakesBodies = nullptr;
public:
	int numberOfUsedSnakes = 0;
	int numberOfAllocatedSnakes;
	int supposedMaxSizeOfSnakeBody;
	Snake* snakes = nullptr;

	/* physw and physh are sizes of gamefield */
	SnakesManager(int physw, int physh, int supposedMaxSizeOfSnakeBody);
	~SnakesManager();
	Snake* NewSnake();
	SnakeBlock* AddBlockToSnake(int snake_id, SnakeBlock snakeBlock);
	void KillSnake(int snake_id);
};