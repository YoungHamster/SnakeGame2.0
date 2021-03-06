#include "GamePlayEngine_server.h"

GamePlayEngine_server::GamePlayEngine_server(int physw, int physh)
	:physw(physw), physh(physh), snakesManager(physw, physh, 200)
{
	number_of_snakes = &snakesManager.numberOfUsedSnakes;
	physics = new PhysicalObject[physw * physh];
	for (int i = 0; i < physw; i++)
	{
		ChangeObject(i, 0, BARRIER);
	}
	for (int i = 0; i < physw; i++)
	{
		ChangeObject(i, physh - 1, BARRIER);
	}
	for (int i = 1; i < physh; i++)
	{
		ChangeObject(0, i, BARRIER);
	}
	for (int i = 1; i < physh; i++)
	{
		ChangeObject(physw - 1, i, BARRIER);
	}
}

GamePlayEngine_server::~GamePlayEngine_server()
{
	delete[] physics;
}

void GamePlayEngine_server::ChangeObject(int x, int y, int type)
{
	if (x < 0 || y < 0 || x > physw || y > physh || GetObjType(x, y) == BARRIER)
	{
		return;
	}
	physics[y * physw + x].type = type;
}

int GamePlayEngine_server::GetObjType(int x, int y)
{
	if (x < 0 || y < 0 || x > physw || y > physh)
	{
		return -1;
	}
	return physics[y * physw + x].type;
}

int GamePlayEngine_server::SpawnSnake(int size, int headx, int heady, char headdir, AITypes aiType)
{
	Snake* newSnake = snakesManager.NewSnake();
	newSnake->newdir = headdir;
	newSnake->aiType = aiType;
	SnakeBlock sb = { headx, heady, headdir };
	for (int i = 0; i < size; i++)
	{
		switch (headdir)
		{
		case UP: sb.y = heady - i; break;
		case DOWN: sb.y = heady + i; break;
		case LEFT: sb.x = headx + i; break;
		case RIGHT: sb.x = headx - i; break;
		}
		snakesManager.AddBlockToSnake(*number_of_snakes - 1, sb);
		ChangeObject(sb.x, sb.y, SNAKE);
	}

	return *number_of_snakes - 1;
}

void GamePlayEngine_server::SplitSnakes(int minLenghtToSplit)
{
	for (int i = 0; i < *number_of_snakes; i++)
	{
		if (snakesManager.snakes[i].bodySize >= minLenghtToSplit)
		{
			SplitSnake(i);
		}
	}
}

void GamePlayEngine_server::SpawnApple()
{
	int x = rand() % physw + 1;
	int y = rand() % physh + 1;
	if (GetObjType(x, y) != 0)
	{
		return;
	}
	ChangeObject(x, y, APPLE);
}

void GamePlayEngine_server::DespawnSnake(int snake_id)
{
	if (*number_of_snakes == 0 || snake_id > * number_of_snakes)
	{
		__debugbreak();
	}
	for (int i = 0; i < snakesManager.snakes[snake_id].bodySize; i++)
	{
		ChangeObject(snakesManager.snakes[snake_id].body[i].x, snakesManager.snakes[snake_id].body[i].y, 0);
	}
	snakesManager.KillSnake(snake_id);
}

void GamePlayEngine_server::FeedSnake(int snake_id)
{
	if (*number_of_snakes == 0 || snake_id > * number_of_snakes)
	{
		__debugbreak();
	}
	SnakeBlock sb = snakesManager.snakes[snake_id].body[snakesManager.snakes[snake_id].bodySize - 1];
	switch (sb.dir)
	{
	case UP: sb.y -= 1; break;
	case DOWN: sb.y += 1; break;
	case LEFT: sb.x += 1; break;
	case RIGHT: sb.x -= 1; break;
	}
	ChangeObject(sb.x, sb.y, SNAKE);
	snakesManager.AddBlockToSnake(snake_id, sb);
}

void GamePlayEngine_server::ShortenSnake(int snake_id)
{
	if (*number_of_snakes == 0 || snake_id > * number_of_snakes)
	{
		__debugbreak();
	}
	// TODO
}

void GamePlayEngine_server::HandleSnakeAI(int snake_id)
{
	AIWeight resultingWeight = { 0, 0, 0, 0 };
	for (int i = 0; i < SNAKE_FOV_HEIGHT; i++)
	{
		for (int j = 0; j < SNAKE_FOV_WIDTH; j++)
		{
			switch (GetObjType(snakesManager.snakes[snake_id].body[0].x + j - SNAKE_FOV_WIDTH / 2, snakesManager.snakes[snake_id].body[0].y + i - SNAKE_FOV_HEIGHT / 2))
			{
			case APPLE: resultingWeight = resultingWeight + foodWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			case BARRIER: resultingWeight = resultingWeight + obstacleWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			case SNAKE: resultingWeight = resultingWeight + obstacleWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			}
		}
	}
	ChangeSnakeDirection(snake_id, resultingWeight.GetDirection());
}

void GamePlayEngine_server::SplitSnake(int snake_id)
{
	Snake* newSnake = snakesManager.NewSnake();
	newSnake->aiType = snakesManager.snakes[snake_id].aiType;
	newSnake->newdir = 0;
	newSnake->texture = snakesManager.snakes[snake_id].texture;
	int newSnakeSize = snakesManager.snakes[snake_id].bodySize / 2 - 1;
	for (int i = 0; i < newSnakeSize; i++)
	{
		snakesManager.AddBlockToSnake(*number_of_snakes - 1, snakesManager.snakes[snake_id].body[newSnakeSize + i + 2]);
	}
	for (int i = 0; i < snakesManager.snakes[snake_id].bodySize / 2; i++)
	{
		ChangeObject(snakesManager.snakes[snake_id].body[snakesManager.snakes[snake_id].bodySize / 2 + i].x,
			snakesManager.snakes[snake_id].body[snakesManager.snakes[snake_id].bodySize / 2 + i].y, 0);
	}
	snakesManager.snakes[snake_id].bodySize -= snakesManager.snakes[snake_id].bodySize / 2;
}

void GamePlayEngine_server::MoveSnakes()
{
	for (int i = 0; i < *number_of_snakes; i++)
	{
		if (snakesManager.snakes[i].aiType != realPlayer) HandleSnakeAI(i);
	}
	for (int i = 0; i < *number_of_snakes; i++)
	{
		int x = snakesManager.snakes[i].body[0].x;
		int y = snakesManager.snakes[i].body[0].y;
		snakesManager.snakes[i].body[0].dir = snakesManager.snakes[i].newdir;
		switch (snakesManager.snakes[i].body[0].dir)
		{
		case UP: y += 1;  break;
		case DOWN: y -= 1;  break;
		case LEFT: x -= 1;  break;
		case RIGHT: x += 1;  break;
		}
		switch (GetObjType(x, y))
		{
		case BARRIER: DespawnSnake(i); i -= 1; continue; break;
		case APPLE: FeedSnake(i); break;
		}
		int snakeAtiSize = snakesManager.snakes[i].bodySize;
		ChangeObject(snakesManager.snakes[i].body[snakeAtiSize - 1].x, snakesManager.snakes[i].body[snakeAtiSize - 1].y, 0);
		ChangeObject(x, y, SNAKE);
		for (int j = 0; j < snakeAtiSize; j++)
		{
			int id = (int)snakeAtiSize - j - 1;
			switch (snakesManager.snakes[i].body[id].dir)
			{
			case UP: snakesManager.snakes[i].body[id].y += 1; break;
			case DOWN: snakesManager.snakes[i].body[id].y -= 1; break;
			case LEFT: snakesManager.snakes[i].body[id].x -= 1; break;
			case RIGHT: snakesManager.snakes[i].body[id].x += 1;  break;
			}
			if (id > 0)
			{
				snakesManager.snakes[i].body[id].dir = snakesManager.snakes[i].body[id - 1].dir;
			}
		}
	}
	bool* isSnakeDead = new bool[*number_of_snakes];
	for (int i = 0; i < *number_of_snakes; i++)
	{
		isSnakeDead[i] = false;
		for (int j = 0; j < *number_of_snakes; j++)
		{
			for (int k = 0; k < snakesManager.snakes[j].bodySize; k++)
			{
				if (i != j || k != 0)
				{
					if (snakesManager.snakes[i].body[0].x == snakesManager.snakes[j].body[k].x
						&& snakesManager.snakes[i].body[0].y == snakesManager.snakes[j].body[k].y)
					{
						isSnakeDead[i] = true;
						k = snakesManager.snakes[j].bodySize;
						j = *number_of_snakes;
					}
				}
			}
		}
	}

	// offset caused by deleting snakes and not deleting values from isSnakeDead array
	int offset = 0;
	for (int i = 0; i < *number_of_snakes; i++)
	{
		if (isSnakeDead[i + offset])
		{
			DespawnSnake(i);
			i -= 1;
			offset += 1;
		}
	}
	delete[] isSnakeDead;
}

void GamePlayEngine_server::SetAppleSpawnFrequency(int applesPerTick)
{
	appleSpawnFrequency = applesPerTick;
}

void GamePlayEngine_server::ChangeSnakeDirection(int snake_id, char dir)
{
	if (*number_of_snakes == 0 || snake_id > * number_of_snakes)
	{
		__debugbreak();
	}
	if ((snakesManager.snakes[snake_id].body[0].dir == UP && dir == DOWN) || (snakesManager.snakes[snake_id].body[0].dir == DOWN && dir == UP) ||
		(snakesManager.snakes[snake_id].body[0].dir == LEFT && dir == RIGHT) || (snakesManager.snakes[snake_id].body[0].dir == RIGHT && dir == LEFT))
	{
		return;
	}
	snakesManager.snakes[snake_id].newdir = dir;
}

void GamePlayEngine_server::StartGame(int numberOfSnakes, int snakesSizes)
{
	KillAllSnakes();
	for (int i = 0; i < physw*physh; i++)
	{
		if (physics[i].type == APPLE) physics[i].type = NOTHING;
	}
	for (int i = 0; i < numberOfSnakes; i++)
	{
		SpawnSnake(snakesSizes, physw/2, physh - 2 - (i *2), LEFT, realPlayer);
	}
}

void GamePlayEngine_server::GameTick()
{
	tickNumber += 1;
	for (int i = 0; i < appleSpawnFrequency; i++)
	{
		SpawnApple();
	}
	MoveSnakes();
}

bool GamePlayEngine_server::IsAnyPlayerAlive()
{
	for (int i = 0; i < *number_of_snakes; i++)
	{
		if (snakesManager.snakes[i].aiType == realPlayer)
		{
			return true;
		}
	}
	return false;
}

void GamePlayEngine_server::KillAllSnakes()
{
	for (int i = 0; i < *number_of_snakes; i++)
	{
		DespawnSnake(i);
		i -= 1;
	}
}

char* GamePlayEngine_server::GetGameData(int* dataSize)
{
	int numberOfApples = GetNumberOfApples();
	*dataSize = sizeof(GameDataHeader) + *number_of_snakes * sizeof(CompressedSnake) + GetNumberOfSnakeBlocks() * sizeof(SnakeBlock) + numberOfApples * sizeof(AppleData);
	char* data = new char[*dataSize];
	GameDataHeader gdh = { tickNumber, *dataSize, sizeof(GameDataHeader), (short)* number_of_snakes, 
		sizeof(GameDataHeader) + *number_of_snakes * sizeof(CompressedSnake), *dataSize - numberOfApples * sizeof(AppleData) };
	*(GameDataHeader*)& data[0] = gdh;
	CompressedSnake snake;
	SnakeBlock snakeBlock;
	unsigned int blockOffset = 0;
	for (int i = 0; i < *number_of_snakes; i++)
	{
		snake = { blockOffset, (short)snakesManager.snakes[i].bodySize };
		*(CompressedSnake*)& data[gdh.snakesOffset + i * sizeof(CompressedSnake)] = snake;
		for (int j = 0; j < snakesManager.snakes[i].bodySize; j++)
		{
			snakeBlock =  snakesManager.snakes[i].body[j];
			*(SnakeBlock*)& data[gdh.snakesBlocksOffset + blockOffset + j * sizeof(SnakeBlock)] = snakeBlock;
		}
		blockOffset += snakesManager.snakes[i].bodySize * sizeof(SnakeBlock);
	}

	int numberOfProcessedApples = 0;
	for (int i = 0; i < physh; i++)
	{
		for (int j = 0; j < physw; j++)
		{
			if (GetObjType(j, i) == APPLE)
			{
				*(AppleData*)& data[gdh.applesOffset + numberOfProcessedApples] = { j, i };
				numberOfProcessedApples += 1;
			}
		}
		// little optimization
		if (numberOfProcessedApples == numberOfApples) return data;
	}
	return data;
}

int GamePlayEngine_server::GetNumberOfSnakes()
{
	return *number_of_snakes;
}

int GamePlayEngine_server::GetNumberOfSnakeBlocks()
{
	int numberOfSnakeBlocks = 0;
	for (int i = 0; i < *number_of_snakes; i++)
	{
		numberOfSnakeBlocks += snakesManager.snakes[i].bodySize;
	}
	return numberOfSnakeBlocks;
}

int GamePlayEngine_server::GetNumberOfApples()
{
	int numberOfApples = 0;
	for (int i = 0; i < physh; i++)
	{
		for (int j = 0; j < physw; j++)
		{
			if (GetObjType(j, i) == APPLE) numberOfApples += 1;
		}
	}
	return numberOfApples;
}

int GamePlayEngine_server::GetPhysicsSize()
{
	return physw*physh;
}

const PhysicalObject * GamePlayEngine_server::GetPhysics()
{
	return physics;
}
