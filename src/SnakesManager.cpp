#include "SnakesManager.h"
#include <stdlib.h>
#include <cstring>

SnakesManager::SnakesManager(int physw, int physh, int supposedMaxSizeOfSnakeBody)
	:numberOfAllocatedSnakes((physw + physh) * 2), supposedMaxSizeOfSnakeBody(supposedMaxSizeOfSnakeBody)
{
	/* Allocate memory for snakes with a big margin */
	snakes = (Snake*)malloc(sizeof(Snake) * numberOfAllocatedSnakes);
	snakesBodies = (SnakeBlock*)malloc(sizeof(SnakeBlock) * numberOfAllocatedSnakes * supposedMaxSizeOfSnakeBody);
	for (int i = 0; i < numberOfAllocatedSnakes; i++)
	{
		snakes[i] = Snake();
		snakes[i].body = snakesBodies + i * supposedMaxSizeOfSnakeBody;
	}
}

SnakesManager::~SnakesManager()
{
	for (int i = 0; i < numberOfAllocatedSnakes; i++)
	{
		if (!snakes[i].bodyAllocatedInCommonBuffer)
		{
			free(snakes[i].body);
			snakes[i].body = nullptr;
		}
	}
	if (snakes)
	{
		free(snakes);
		snakes = nullptr;
	}
	if (snakesBodies)
	{
		free(snakesBodies);
		snakesBodies = nullptr;
	}
}

Snake* SnakesManager::NewSnake()
{
	if (numberOfUsedSnakes == numberOfAllocatedSnakes)
	{
		snakes = (Snake*)realloc(snakes, sizeof(Snake) * numberOfAllocatedSnakes * 2);
		numberOfAllocatedSnakes = numberOfAllocatedSnakes * 2;
	}
	snakes[numberOfUsedSnakes] = Snake();// initialize new snake with default value
	snakes[numberOfUsedSnakes].body = snakesBodies + numberOfUsedSnakes * supposedMaxSizeOfSnakeBody;
	numberOfUsedSnakes += 1;
	return &snakes[numberOfUsedSnakes - 1];
}

SnakeBlock* SnakesManager::AddBlockToSnake(int snake_id, SnakeBlock snakeBlock)
{
	if (snakes[snake_id].bodyAllocatedInCommonBuffer)
	{
		if (snakes[snake_id].bodySize == supposedMaxSizeOfSnakeBody)
		{
			/* Previously .body pointed to some part of snakesBodies array, 
			so we do not free it, we just create another array just for this snake, because it's too big */
			SnakeBlock* newBody = (SnakeBlock*)malloc(sizeof(SnakeBlock) * supposedMaxSizeOfSnakeBody * 2); // Allocate twice as much as it was previously
			memcpy(newBody, snakes[snake_id].body, snakes[snake_id].bodySize * sizeof(SnakeBlock));
			snakes[snake_id].body = newBody;
			snakes[snake_id].bodyAllocatedInCommonBuffer = false;
			snakes[snake_id].sizeAllocatedForBody = supposedMaxSizeOfSnakeBody * 2;
		}
	}
	else
	{
		if (snakes[snake_id].bodySize == snakes[snake_id].sizeAllocatedForBody)
		{
			snakes[snake_id].body = (SnakeBlock*)realloc(snakes[snake_id].body, sizeof(SnakeBlock) * snakes[snake_id].sizeAllocatedForBody * 2); // Allocate twice as much as it was previously
			snakes[snake_id].sizeAllocatedForBody = snakes[snake_id].sizeAllocatedForBody * 2;
		}
	}
	snakes[snake_id].body[snakes[snake_id].bodySize] = snakeBlock;
	snakes[snake_id].bodySize += 1;
	return &snakes[snake_id].body[snakes[snake_id].bodySize - 1];
}

void SnakesManager::KillSnake(int snake_id)
{
	if (!snakes[snake_id].bodyAllocatedInCommonBuffer)
	{
		free(snakes[snake_id].body);
		snakes[snake_id].body = nullptr;
		snakes[snake_id].bodyAllocatedInCommonBuffer = true;
	}
	for (int i = 0; i < numberOfUsedSnakes - snake_id - 1; i++)
	{
		if (snakes[snake_id + 1].bodyAllocatedInCommonBuffer)
		{
			memcpy(&snakesBodies[(snake_id + i) * supposedMaxSizeOfSnakeBody],
				&snakesBodies[(snake_id + i + 1) * supposedMaxSizeOfSnakeBody],
				snakes[snake_id + i + 1].bodySize * sizeof(SnakeBlock));
			snakes[snake_id + i + 1].body = &snakesBodies[(snake_id + i) * supposedMaxSizeOfSnakeBody];
		}
	}
	memcpy(&snakes[snake_id], &snakes[snake_id + 1], sizeof(Snake) * (numberOfUsedSnakes - snake_id - 1));
	numberOfUsedSnakes -= 1;
}
