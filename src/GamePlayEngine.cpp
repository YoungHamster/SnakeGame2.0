#include "GamePlayEngine.h"

GamePlayEngine::GamePlayEngine(int physw, int physh)
	:physw(physw), physh(physh), snakes(sizeof(Snake))
{
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
	snakes.~DynamicArray();
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
	Snake *newSnake = new Snake();
	snakes.PushBack(newSnake);
	number_of_snakes += 1;
	GetSnake(number_of_snakes - 1)->snake.Resize(size);
	GetSnake(number_of_snakes - 1)->newdir = headdir;
	GetSnake(number_of_snakes - 1)->aiType = aiType;
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
		*GetSnakeBlock(number_of_snakes - 1, i) = sb;
		ChangeObject(sb.x, sb.y, SNAKE);
	}

	return number_of_snakes - 1;
}

void GamePlayEngine::SplitSnakes(int minLenghtToSplit, bool evolutionaryMode)
{
	for (int i = 0; i < number_of_snakes; i++)
	{
		if (((Snake*)snakes[i])->snake.Size() >= minLenghtToSplit)
		{
			SplitSnake(i, evolutionaryMode);
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
	if (number_of_snakes == 0 || snake_id > number_of_snakes)
	{
		return;
	}
	for (int i = 0; i < GetSnake(snake_id)->snake.Size(); i++)
	{
		ChangeObject(GetSnakeBlock(snake_id, i)->x, GetSnakeBlock(snake_id, i)->y, 0);
	}
	snakes.Delete(snake_id);
	number_of_snakes -= 1;
}

void GamePlayEngine::FeedSnake(int snake_id)
{
	if (number_of_snakes == 0 || snake_id > number_of_snakes)
	{
		return;
	}
	SnakeBlock sb = *GetSnakeBlock(snake_id, GetSnake(snake_id)->snake.Size() - 1);
	switch (sb.dir)
	{
	case UP: sb.y -= 1; break;
	case DOWN: sb.y += 1; break;
	case LEFT: sb.x += 1; break;
	case RIGHT: sb.x -= 1; break;
	}
	ChangeObject(sb.x, sb.y, SNAKE);
	GetSnake(snake_id)->snake.PushBack(&sb);
}

void GamePlayEngine::ShortenSnake(int snake_id)
{
	if (number_of_snakes == 0 || snake_id > number_of_snakes)
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
			switch (GetObjType( GetSnakeBlock(snake_id, 0)->x + j - SNAKE_FOV_WIDTH / 2, GetSnakeBlock(snake_id, 0)->y + i - SNAKE_FOV_HEIGHT / 2))
			{
			case APPLE: resultingWeight = resultingWeight + GetSnake(snake_id)->foodWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			case BARRIER: resultingWeight = resultingWeight + GetSnake(snake_id)->obstacleWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			case SNAKE: resultingWeight = resultingWeight + GetSnake(snake_id)->obstacleWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			}
		}
	}
	ChangeSnakeDirection(snake_id, resultingWeight.GetDirection());
}

void GamePlayEngine::SplitSnake(int snake_id, bool evolutionaryMode)
{
	snakes.PushBack(&Snake());
	number_of_snakes += 1;	
	GetSnake(number_of_snakes - 1)->aiType = GetSnake(snake_id)->aiType;
	GetSnake(number_of_snakes - 1)->weightsWereChanged = GetSnake(snake_id)->weightsWereChanged;
	for (int i = 0; i < SNAKE_FOV_HEIGHT; i++)
	{
		for (int j = 0; j < SNAKE_FOV_WIDTH; j++)
		{
			GetSnake(number_of_snakes - 1)->foodWeights[i][j] = GetSnake(snake_id)->foodWeights[i][j];
			GetSnake(number_of_snakes - 1)->obstacleWeights[i][j] = GetSnake(snake_id)->obstacleWeights[i][j];
		}
	}
	GetSnake(number_of_snakes - 1)->newdir = 0;
	GetSnake(number_of_snakes - 1)->texture = GetSnake(snake_id)->texture;

	GetSnake(number_of_snakes - 1)->snake.Resize(GetSnake(snake_id)->snake.Size() / 2 - 1);
	for (int i = 0; i < ((Snake*)snakes[number_of_snakes - 1])->snake.Size(); i++)
	{
		*GetSnakeBlock(number_of_snakes - 1, i) = *GetSnakeBlock(snake_id, GetSnake(snake_id)->snake.Size() / 2 + i - 1);
	}

	GetSnake(snake_id)->snake.Delete( GetSnake(snake_id)->snake.Size() / 2, GetSnake(snake_id)->snake.Size() - 1);
}

void GamePlayEngine::MoveSnakes()
{
	for (int i = 0; i < number_of_snakes; i++)
	{
		if(((Snake*)snakes[i])->aiType != realPlayer) HandleSnakeAI(i);
	}
	for (int i = 0; i < number_of_snakes; i++)
	{
		int x = GetSnakeBlock(i, 0)->x;
		int y = GetSnakeBlock(i, 0)->y;
		GetSnakeBlock(i, 0)->dir = GetSnake(i)->newdir;
		switch (GetSnakeBlock(i, 0)->dir)
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
		size_t snakeAtiSize = GetSnake(i)->snake.Size();
		ChangeObject(GetSnakeBlock(i, snakeAtiSize - 1)->x, GetSnakeBlock(i, snakeAtiSize - 1)->y, 0);
		ChangeObject(x, y, SNAKE);
		for (int j = 0; j < snakeAtiSize; j++)
		{
			int id = (int)snakeAtiSize - j - 1;
			switch (GetSnakeBlock(i, id)->dir)
			{
			case UP: GetSnakeBlock(i, id)->y += 1; break;
			case DOWN: GetSnakeBlock(i, id)->y -= 1; break;
			case LEFT: GetSnakeBlock(i, id)->x -= 1; break;
			case RIGHT: GetSnakeBlock(i, id)->x += 1;  break;
			}
			if (id >= 1)
			{
				GetSnakeBlock(i, id)->dir = GetSnakeBlock(i, id - 1)->dir;
			}
		}
	}

	// This this code checks if snakes collide with any other snakes and their own bodies
	//std::vector<SnakeBlock> allSnakeBlocksWithoutHeads; // sorry for long name
	DynamicArray allSnakeBlocksWithoutHeads(sizeof(SnakeBlock));
	SnakeBlock *snakesHeads = new SnakeBlock[snakes.Size()];
	for (int i = 0; i < number_of_snakes; i++)
	{
		snakesHeads[i] = *GetSnakeBlock(i, 0);
		size_t prevSize = allSnakeBlocksWithoutHeads.Size();
		size_t snakeAtiSize = GetSnake(i)->snake.Size();
		allSnakeBlocksWithoutHeads.Resize(allSnakeBlocksWithoutHeads.Size() + snakeAtiSize - 1);
		for (int j = 1; j < snakeAtiSize; j++)
		{
			*(SnakeBlock*)allSnakeBlocksWithoutHeads[prevSize + j - 1] = *GetSnakeBlock(i, j);
		}
	}
	size_t allSnakeBlocksWithoutHeadsSize = allSnakeBlocksWithoutHeads.Size();
	for (int i = 0; i < number_of_snakes; i++)
	{
		size_t snakeAtiSize = GetSnake(i)->snake.Size();
		for (int j = 0; j < number_of_snakes; j++)
		{
			// if i == j it means, that we try to compare snake's head with itself
			if (i != j && snakeAtiSize > 0)
			{
				if (GetSnakeBlock(i, 0)->x == snakesHeads[j].x && GetSnakeBlock(i, 0)->y == snakesHeads[j].y)
				{
					GetSnake(i)->snake.Clear(); // clear snake instead of Despawning it to keep this algorithm easy
					snakeAtiSize = 0;
					j = number_of_snakes;
				}
			}
		}
		for (int j = 0; j < allSnakeBlocksWithoutHeadsSize; j++)
		{
			if (snakeAtiSize > 0)
			{
				if (GetSnakeBlock(i, 0)->x == ((SnakeBlock*)allSnakeBlocksWithoutHeads[j])->x && GetSnakeBlock(i, 0)->y == ((SnakeBlock*)allSnakeBlocksWithoutHeads[j])->y)
				{
					GetSnake(i)->snake.Clear();
					j = allSnakeBlocksWithoutHeadsSize;
				}
			}
		}
	}

	// delete all empty snakes
	for (int i = 0; i < number_of_snakes; i++)
	{
		if (GetSnake(i)->snake.Size() == 0)
		{
			DespawnSnake(i);
			i -= 1;
		}
	}
}

void GamePlayEngine::ChangeSnakeDirection(int snake_id, char dir)
{
	if (number_of_snakes == 0 || snake_id > number_of_snakes)
	{
		return;
	}
	if ((GetSnakeBlock(snake_id, 0)->dir == UP && dir == DOWN) || (GetSnakeBlock(snake_id, 0)->dir == DOWN && dir == UP) ||
		(GetSnakeBlock(snake_id, 0)->dir == LEFT && dir == RIGHT) || (GetSnakeBlock(snake_id, 0)->dir == RIGHT && dir == LEFT))
	{
		return;
	}
	((Snake*)snakes[snake_id])->newdir = dir;
}

Snake * GamePlayEngine::GetSnake(int snake_id)
{
	return (Snake*)snakes.GetArray() + snake_id;
}

SnakeBlock * GamePlayEngine::GetSnakeBlock(int snake_id, int block_id)
{
	return (SnakeBlock*)GetSnake(snake_id)->snake.GetArray() + block_id;
}

FrameRenderingInput GamePlayEngine::GetFrameRenderingInput()
{
	return { physics, physw, physh, &snakes };
}

int GamePlayEngine::GetNumberOfSnakes()
{
	return number_of_snakes;
}
