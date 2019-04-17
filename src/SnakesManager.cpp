#include "SnakesManager.h"
#include <stdlib.h>
#include <cstring>

SnakesManager::SnakesManager(int physw, int physh, int supposedMaxSizeOfSnakeBody)
	:numberOfAllocatedSnakes((physw + physh) * 4), supposedMaxSizeOfSnakeBody(supposedMaxSizeOfSnakeBody)
{
	/* Allocate memory for snakes with a big margin */
	snakes = (Snake*)malloc(sizeof(Snake) * numberOfAllocatedSnakes);
	snakesBodies = (SnakeBlock*)malloc(sizeof(SnakeBlock) * numberOfAllocatedSnakes * supposedMaxSizeOfSnakeBody);
	if (snakes == nullptr || snakesBodies == nullptr) __debugbreak();
	for (int i = 0; i < numberOfAllocatedSnakes; i++)
	{
		snakes[i] = Snake();
		snakes[i].body = snakesBodies + (unsigned long long)i * supposedMaxSizeOfSnakeBody;
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
		Snake* reallocatedSnakes = (Snake*)realloc(snakes, sizeof(Snake) * numberOfAllocatedSnakes * 2);
		if (reallocatedSnakes == nullptr) __debugbreak();
		snakes = reallocatedSnakes;
		numberOfAllocatedSnakes = numberOfAllocatedSnakes * 2;
		SnakeBlock* reallocatedSnakesBodies = (SnakeBlock*)realloc(snakesBodies,
			sizeof(SnakeBlock) * numberOfAllocatedSnakes * supposedMaxSizeOfSnakeBody);
		if (reallocatedSnakesBodies == nullptr) __debugbreak();
		snakesBodies = reallocatedSnakesBodies;
		for (int i = 0; i < numberOfUsedSnakes; i++)
		{
			if (snakes[i].bodyAllocatedInCommonBuffer)
			{
				snakes[i].body = snakesBodies + supposedMaxSizeOfSnakeBody * i;
			}
		}
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
			if (newBody == nullptr) __debugbreak();
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
			SnakeBlock* reallocatedBody = (SnakeBlock*)realloc(snakes[snake_id].body, sizeof(SnakeBlock) * snakes[snake_id].sizeAllocatedForBody * 2); // Allocate twice as much as it was previously
			if (reallocatedBody == nullptr) __debugbreak();
			snakes[snake_id].body = reallocatedBody;
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
		snakes[snake_id] = Snake();
	}
	for (int i = 0; i < numberOfUsedSnakes - snake_id - 1; i++)
	{
		if (snakes[snake_id + 1].bodyAllocatedInCommonBuffer)
		{
			memcpy(snakesBodies + (snake_id + i) * supposedMaxSizeOfSnakeBody,
				snakesBodies + (snake_id + i + 1) * supposedMaxSizeOfSnakeBody,
				snakes[snake_id + i + 1].bodySize * sizeof(SnakeBlock));
			snakes[snake_id + i + 1].body = snakesBodies + (snake_id + i) * supposedMaxSizeOfSnakeBody;
		}
	}
	memcpy(&snakes[snake_id], &snakes[snake_id + 1], sizeof(Snake) * (numberOfUsedSnakes - snake_id - 1));
	numberOfUsedSnakes -= 1;
}
