#include "GamePlayEngine.h"

GamePlayEngine::GamePlayEngine(int physw, int physh)
	:physw(physw), physh(physh), snakesManager(physw, physh, 200)
{
	number_of_snakes = &snakesManager.numberOfUsedSnakes;
	physics = new PhysicalObject[physw*physh];
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

GamePlayEngine::~GamePlayEngine()
{
	delete[] physics;
}

void GamePlayEngine::ChangeObject(int x, int y, int type) 
{
	if (x < 0 || y < 0 || x > physw || y > physh || GetObjType(x, y) == BARRIER)
	{
		return;
	}
	physics[y*physw + x].type = type;
}

int GamePlayEngine::GetObjType(int x, int y)
{
	if (x < 0 || y < 0 || x > physw || y > physh)
	{
		return -1;
	}
	return physics[y*physw + x].type;
}

int GamePlayEngine::SpawnSnake(int size, int headx, int heady, char headdir, AITypes aiType)
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

void GamePlayEngine::SplitSnakes(int minLenghtToSplit)
{
	for (int i = 0; i < *number_of_snakes; i++)
	{
		if (snakesManager.snakes[i].bodySize >= minLenghtToSplit)
		{
			SplitSnake(i);
		}
	}
}

void GamePlayEngine::SpawnApple()
{
	int x = rand() % physw + 1;
	int y = rand() % physh + 1;
	if (GetObjType(x, y) != 0)
	{
		return;
	}
	ChangeObject(x, y, APPLE);
}

void GamePlayEngine::DespawnSnake(int snake_id)
{
	if (*number_of_snakes == 0 || snake_id > *number_of_snakes)
	{
		return;
	}
	for (int i = 0; i < snakesManager.snakes[snake_id].bodySize; i++)
	{
		ChangeObject(snakesManager.snakes[snake_id].body[i].x, snakesManager.snakes[snake_id].body[i].y, 0);
	}
	snakesManager.KillSnake(snake_id);
	*number_of_snakes -= 1;
}

void GamePlayEngine::FeedSnake(int snake_id)
{
	if (*number_of_snakes == 0 || snake_id > *number_of_snakes)
	{
		return;
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

void GamePlayEngine::ShortenSnake(int snake_id)
{
	if (*number_of_snakes == 0 || snake_id > *number_of_snakes)
	{
		return;
	}
	// TODO
}

void GamePlayEngine::HandleSnakeAI(int snake_id)
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

void GamePlayEngine::SplitSnake(int snake_id)
{
	Snake* newSnake = snakesManager.NewSnake();
	newSnake->aiType = snakesManager.snakes[snake_id].aiType;
	newSnake->newdir = 0;
	newSnake->texture = snakesManager.snakes[snake_id].texture;
	int newSnakeSize = snakesManager.snakes[snake_id].bodySize / 2 - 1;
	for (int i = 0; i < newSnakeSize; i++)
	{
		snakesManager.AddBlockToSnake(*number_of_snakes - 1, snakesManager.snakes[snake_id].body[newSnakeSize + i - 1]);
	}
	snakesManager.CutTailOfSnake(snake_id, snakesManager.snakes[snake_id].bodySize / 2);
}

void GamePlayEngine::MoveSnakes()
{
	for (int i = 0; i < *number_of_snakes; i++)
	{
		if(snakesManager.snakes[i].aiType != realPlayer) HandleSnakeAI(i);
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
			if (id >= 1)
			{
				snakesManager.snakes[i].body[id].dir = snakesManager.snakes[i].body[id - 1].dir;
			}
		}
	}

	// This this code checks if snakes collide with any other snakes and their own bodies
	SnakeBlock* allSnakeBlocksWithoutHeads; // sorry for long name
	int allSnakeBlocksWithoutHeadsSize = 0;
	for (int i = 0; i < *number_of_snakes; i++)
	{
		allSnakeBlocksWithoutHeadsSize += snakesManager.snakes[i].bodySize;
	}
	allSnakeBlocksWithoutHeads = new SnakeBlock[allSnakeBlocksWithoutHeadsSize];
	SnakeBlock *snakesHeads = new SnakeBlock[*number_of_snakes];
	int prevSize = 0;
	for (int i = 0; i < *number_of_snakes; i++)
	{
		snakesHeads[i] = snakesManager.snakes[i].body[0];
		memcpy(allSnakeBlocksWithoutHeads + prevSize, snakesManager.snakes[i].body + 1, (snakesManager.snakes[i].bodySize - 1) * sizeof(SnakeBlock));
		prevSize += snakesManager.snakes[i].bodySize;
	}
	for (int i = 0; i < *number_of_snakes; i++)
	{
		int snakeAtiSize = snakesManager.snakes[i].bodySize;
		for (int j = 0; j < *number_of_snakes; j++)
		{
			// if i == j it means, that we try to compare snake's head with itself
			if (i != j && snakeAtiSize > 0)
			{
				if (snakesManager.snakes[i].body[0].x == snakesHeads[j].x && snakesManager.snakes[i].body[0].y == snakesHeads[j].y)
				{
					
					snakeAtiSize = 0; // set size of snake to 0 instead of Despawning it to keep this algorithm easy
					j = *number_of_snakes;
				}
			}
		}
		for (int j = 0; j < allSnakeBlocksWithoutHeadsSize; j++)
		{
			if (snakeAtiSize > 0)
			{
				if (snakesManager.snakes[i].body[0].x == allSnakeBlocksWithoutHeads[j].x && snakesManager.snakes[i].body[0].y == allSnakeBlocksWithoutHeads[j].y)
				{
					snakeAtiSize = 0;
					j = allSnakeBlocksWithoutHeadsSize;
				}
			}
		}
		/* Clear physics from dead snake, because DespawnSnake() wont do it with bodySize set to 0 */
		if (snakeAtiSize == 0)
		{
			for (int j = 0; j < snakesManager.snakes[i].bodySize; j++)
			{
				ChangeObject(snakesManager.snakes[i].body[j].x, snakesManager.snakes[i].body[j].y, 0);
			}
			snakesManager.snakes[i].bodySize = 0;
		}
	}
	delete[] allSnakeBlocksWithoutHeads;
	delete[] snakesHeads;

	// delete all empty snakes
	for (int i = 0; i < *number_of_snakes; i++)
	{
		if (snakesManager.snakes[i].bodySize == 0)
		{
			DespawnSnake(i);
			i -= 1;
		}
	}
}

void GamePlayEngine::ChangeSnakeDirection(int snake_id, char dir)
{
	if (*number_of_snakes == 0 || snake_id > *number_of_snakes)
	{
		return;
	}
	if ((snakesManager.snakes[snake_id].body[0].dir == UP && dir == DOWN) || (snakesManager.snakes[snake_id].body[0].dir == DOWN && dir == UP) ||
		(snakesManager.snakes[snake_id].body[0].dir == LEFT && dir == RIGHT) || (snakesManager.snakes[snake_id].body[0].dir == RIGHT && dir == LEFT))
	{
		return;
	}
	snakesManager.snakes[snake_id].newdir = dir;
}

FrameRenderingInput GamePlayEngine::GetFrameRenderingInput()
{
	FrameRenderingInput input;
	input.physics = physics;
	input.physicsWidth = physw;
	input.physicsHeight = physh;
	input.numberOfSnakes = *number_of_snakes;
	input.snakes = new Snake[*number_of_snakes];
	memcpy(input.snakes, snakesManager.snakes, *number_of_snakes * sizeof(Snake));
	int sizeOfSnakesBodies = 0;
	for (int i = 0; i < *number_of_snakes; i++)
	{
		sizeOfSnakesBodies += input.snakes[i].bodySize;
	}
	input.snakesBodies = new SnakeBlock[sizeOfSnakesBodies];
	int prevSize = 0;
	for (int i = 0; i < *number_of_snakes; i++)
	{
		memcpy(input.snakesBodies + prevSize, snakesManager.snakes[i].body, snakesManager.snakes[i].bodySize * sizeof(SnakeBlock));
		input.snakes[i].body = input.snakesBodies + prevSize;
		prevSize += snakesManager.snakes[i].bodySize;
	}
	return input;
}

int GamePlayEngine::GetNumberOfSnakes()
{
	return *number_of_snakes;
}
