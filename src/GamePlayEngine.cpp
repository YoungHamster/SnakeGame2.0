#include "GamePlayEngine.h"

GamePlayEngine::GamePlayEngine(int physw, int physh)
	:physw(physw), physh(physh)
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
	snakes.clear();
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
	snakes.push_back(Snake());
	number_of_snakes += 1;
	snakes.at(number_of_snakes - 1).snake.resize(size);
	snakes.at(number_of_snakes - 1).newdir = headdir;
	snakes.at(number_of_snakes - 1).aiType = aiType;
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
		snakes.at(number_of_snakes - 1).snake[i] = sb;
		ChangeObject(sb.x, sb.y, SNAKE);
	}

	return number_of_snakes - 1;
}

void GamePlayEngine::SplitSnakes(int minLenghtToSplit, bool evolutionaryMode)
{
	for (int i = 0; i < number_of_snakes; i++)
	{
		if (snakes[i].snake.size() >= minLenghtToSplit)
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
	for (int i = 0; i < snakes[snake_id].snake.size(); i++)
	{
		ChangeObject(snakes[snake_id].snake[i].x, snakes[snake_id].snake[i].y, 0);
	}
	snakes.erase(snakes.begin() + snake_id);
	number_of_snakes -= 1;
}

void GamePlayEngine::FeedSnake(int snake_id)
{
	if (number_of_snakes == 0 || snake_id > number_of_snakes)
	{
		return;
	}
	SnakeBlock sb = snakes.at(snake_id).snake[snakes.at(snake_id).snake.size() - 1];
	switch (sb.dir)
	{
	case UP: sb.y -= 1; break;
	case DOWN: sb.y += 1; break;
	case LEFT: sb.x += 1; break;
	case RIGHT: sb.x -= 1; break;
	}
	ChangeObject(sb.x, sb.y, SNAKE);
	snakes.at(snake_id).snake.push_back(sb);
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
			switch (GetObjType(snakes[snake_id].snake[0].x + j - SNAKE_FOV_WIDTH / 2, snakes[snake_id].snake[0].y + i - SNAKE_FOV_HEIGHT / 2))
			{
			case APPLE: resultingWeight = resultingWeight + snakes[snake_id].foodWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			case BARRIER: resultingWeight = resultingWeight + snakes[snake_id].obstacleWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			case SNAKE: resultingWeight = resultingWeight + snakes[snake_id].obstacleWeights[SNAKE_FOV_HEIGHT - i - 1][j]; break;
			}
		}
	}
	ChangeSnakeDirection(snake_id, resultingWeight.GetDirection());
}

void GamePlayEngine::SplitSnake(int snake_id, bool evolutionaryMode)
{
	snakes.push_back(Snake());
	number_of_snakes += 1;	
	snakes[number_of_snakes - 1].aiType = snakes[snake_id].aiType;
	snakes[number_of_snakes - 1].weightsWereChanged = snakes[snake_id].weightsWereChanged;
	for (int i = 0; i < SNAKE_FOV_HEIGHT; i++)
	{
		for (int j = 0; j < SNAKE_FOV_WIDTH; j++)
		{
			snakes[number_of_snakes - 1].foodWeights[i][j] = snakes[snake_id].foodWeights[i][j];
			snakes[number_of_snakes - 1].obstacleWeights[i][j] = snakes[snake_id].obstacleWeights[i][j];
			if (evolutionaryMode)
			{
				if (rand() % 10 == 1)//chance 1 to 10
				{
					switch (rand() % 8)
					{
					case 0: 
						snakes[number_of_snakes - 1].foodWeights[i][j].up = snakes[number_of_snakes - 1].foodWeights[i][j].up + rand() % 2 - 1;
						break;
					case 1: 
						snakes[number_of_snakes - 1].foodWeights[i][j].down = snakes[number_of_snakes - 1].foodWeights[i][j].down + rand() % 2 - 1;
						break;
					case 2: 
						snakes[number_of_snakes - 1].foodWeights[i][j].left = snakes[number_of_snakes - 1].foodWeights[i][j].left + rand() % 2 - 1;
						break;
					case 3: 
						snakes[number_of_snakes - 1].foodWeights[i][j].right = snakes[number_of_snakes - 1].foodWeights[i][j].right + rand() % 2 - 1;
						break;
					case 4: 
						snakes[number_of_snakes - 1].obstacleWeights[i][j].up = snakes[number_of_snakes - 1].obstacleWeights[i][j].up + rand() % 2 - 1;
						break;
					case 5:
						snakes[number_of_snakes - 1].obstacleWeights[i][j].down = snakes[number_of_snakes - 1].obstacleWeights[i][j].down + rand() % 2 - 1;
						break;
					case 6:
						snakes[number_of_snakes - 1].obstacleWeights[i][j].left = snakes[number_of_snakes - 1].obstacleWeights[i][j].left + rand() % 2 - 1;
						break;
					case 7:
						snakes[number_of_snakes - 1].obstacleWeights[i][j].right = snakes[number_of_snakes - 1].obstacleWeights[i][j].right + rand() % 2 - 1;
						break;
					}
					snakes[number_of_snakes - 1].weightsWereChanged = true;
				}
			}
		}
	}
	snakes[number_of_snakes - 1].newdir = 0;
	snakes[number_of_snakes - 1].texture = snakes[snake_id].texture;

	snakes[number_of_snakes - 1].snake.resize(snakes[snake_id].snake.size() / 2 - 1);
	for (int i = 0; i < snakes[number_of_snakes - 1].snake.size(); i++)
	{
		snakes[number_of_snakes - 1].snake[i] = snakes[snake_id].snake[snakes[snake_id].snake.size() / 2 + i - 1];
	}

	snakes[snake_id].snake.erase(snakes[snake_id].snake.begin() + snakes[snake_id].snake.size() / 2, snakes[snake_id].snake.end());
}

void GamePlayEngine::MoveSnakes()
{
	for (int i = 0; i < number_of_snakes; i++)
	{
		if(snakes[i].aiType != realPlayer) HandleSnakeAI(i);
	}
	for (int i = 0; i < number_of_snakes; i++)
	{
		int x = snakes.at(i).snake[0].x;
		int y = snakes.at(i).snake[0].y;
		snakes[i].snake[0].dir = snakes[i].newdir;
		switch (snakes.at(i).snake[0].dir)
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
		size_t snakeAtiSize = snakes.at(i).snake.size();
		ChangeObject(snakes.at(i).snake[snakeAtiSize - 1].x, snakes.at(i).snake[snakeAtiSize - 1].y, 0);
		ChangeObject(x, y, SNAKE);
		for (int j = 0; j < snakeAtiSize; j++)
		{
			int id = (int)snakeAtiSize - j - 1;
			switch (snakes.at(i).snake[id].dir)
			{
			case UP: snakes.at(i).snake[id].y += 1; break;
			case DOWN: snakes.at(i).snake[id].y -= 1; break;
			case LEFT: snakes.at(i).snake[id].x -= 1; break;
			case RIGHT: snakes.at(i).snake[id].x += 1;  break;
			}
			if (id >= 1)
			{
				snakes.at(i).snake[id].dir = snakes.at(i).snake[id - 1].dir;
			}
		}
	}

	// This this code checks if snakes collide with any other snakes and their own bodies
	std::vector<SnakeBlock> allSnakeBlocksWithoutHeads; // sorry for long name
	SnakeBlock *snakesHeads = new SnakeBlock[snakes.size()];
	for (int i = 0; i < number_of_snakes; i++)
	{
		snakesHeads[i] = snakes[i].snake[0];
		size_t prevSize = allSnakeBlocksWithoutHeads.size();
		size_t snakeAtiSize = snakes[i].snake.size();
		allSnakeBlocksWithoutHeads.resize(allSnakeBlocksWithoutHeads.size() + snakeAtiSize - 1);
		for (int j = 1; j < snakeAtiSize; j++)
		{
			allSnakeBlocksWithoutHeads[prevSize + j - 1] = snakes[i].snake[j];
		}
	}
	size_t allSnakeBlocksWithoutHeadsSize = allSnakeBlocksWithoutHeads.size();
	for (int i = 0; i < number_of_snakes; i++)
	{
		size_t snakeAtiSize = snakes[i].snake.size();
		for (int j = 0; j < number_of_snakes; j++)
		{
			// if i == j it means, that we try to compare snake's head with itself
			if (i != j && snakeAtiSize > 0)
			{
				if (snakes[i].snake[0].x == snakesHeads[j].x && snakes[i].snake[0].y == snakesHeads[j].y)
				{
					snakes[i].snake.clear(); // clear snake instead of Despawning it to keep this algorithm easy
					snakeAtiSize = 0;
					j = number_of_snakes;
				}
			}
		}
		for (int j = 0; j < allSnakeBlocksWithoutHeadsSize; j++)
		{
			if (snakeAtiSize > 0)
			{
				if (snakes[i].snake[0].x == allSnakeBlocksWithoutHeads[j].x && snakes[i].snake[0].y == allSnakeBlocksWithoutHeads[j].y)
				{
					snakes[i].snake.clear();
					j = allSnakeBlocksWithoutHeadsSize;
				}
			}
		}
	}

	// delete all empty snakes
	for (int i = 0; i < number_of_snakes; i++)
	{
		if (snakes[i].snake.size() == 0)
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
	if ((snakes[snake_id].snake[0].dir == UP && dir == DOWN) || (snakes[snake_id].snake[0].dir == DOWN && dir == UP) || 
		(snakes[snake_id].snake[0].dir == LEFT && dir == RIGHT) || (snakes[snake_id].snake[0].dir == RIGHT && dir == LEFT))
	{
		return;
	}
	snakes.at(snake_id).newdir = dir;
}

FrameRenderingInput GamePlayEngine::GetFrameRenderingInput()
{
	return { physics, physw, physh, &snakes };
}

int GamePlayEngine::GetNumberOfSnakes()
{
	return number_of_snakes;
}
