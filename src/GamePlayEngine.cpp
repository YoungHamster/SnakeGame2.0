#include "GamePlayEngine.h"

GamePlayEngine::GamePlayEngine(int physw, int physh)
{
	this->physw = physw;
	this->physh = physh;
	physics = new PhysicalObject[physw*physh];
	for (int i = 0; i < physw; i++)
	{
		ChangeObject(i, 0, 1, BARRIER);
	}
	for (int i = 0; i < physw; i++)
	{
		ChangeObject(i, physh - 1, 1, BARRIER);
	}
	for (int i = 1; i < physh; i++)
	{
		ChangeObject(0, i, 1, BARRIER);
	}
	for (int i = 1; i < physh; i++)
	{
		ChangeObject(physw - 1, i, 1, BARRIER);
	}
}

GamePlayEngine::~GamePlayEngine()
{
	delete[] physics;
	snakes.clear();
}

void GamePlayEngine::ChangeObject(int x, int y, int texture, int type) { physics[y*physw + x].texture = texture; physics[y*physw + x].type = type; }

int GamePlayEngine::GetObjType(int x, int y) { return physics[y*physw + x].type; }

int GamePlayEngine::SpawnSnake(int size, int headx, int heady, char headdir)
{
	// TODO params verification
	snakes.push_back(Snake());
	number_of_snakes += 1;
	snakes.at(number_of_snakes - 1).snake.resize(size);
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
		ChangeObject(sb.x, sb.y, 1/*TODO*/, SNAKE);
	}
	return number_of_snakes - 1;
}

void GamePlayEngine::DespawnSnake(int snake_id)
{
	// TODO params verification
	for (int i = 0; i < snakes[snake_id].snake.size(); i++)
	{
		ChangeObject(snakes[snake_id].snake[i].x, snakes[snake_id].snake[i].y, 1, 0);
	}
	snakes.erase(snakes.begin() + snake_id);
	number_of_snakes -= 1;
}

void GamePlayEngine::FeedSnake(int snake_id)
{
	// TODO params verification
	SnakeBlock sb;
	sb.dir = snakes.at(snake_id).snake[snakes.at(snake_id).snake.size() - 1].dir;
	sb.x = snakes.at(snake_id).snake[snakes.at(snake_id).snake.size() - 1].x;
	sb.y = snakes.at(snake_id).snake[snakes.at(snake_id).snake.size() - 1].y;
	switch (sb.dir)
	{
	case UP: sb.y -= 1; break;
	case DOWN: sb.y += 1; break;
	case LEFT: sb.x += 1; break;
	case RIGHT: sb.x -= 1; break;
	}
	ChangeObject(sb.x, sb.y, 1/*TODO*/, SNAKE);
	snakes.at(snake_id).snake.push_back(sb);
}

void GamePlayEngine::ShortenSnake(int snake_id)
{
	// TODO
}

void GamePlayEngine::MoveSnakes()
{
	for (int i = 0; i < snakes.size(); i++)
	{
		int x = snakes.at(i).snake[0].x;
		int y = snakes.at(i).snake[0].y;
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
		ChangeObject(snakes.at(i).snake[snakes.at(i).snake.size() - 1].x, snakes.at(i).snake[snakes.at(i).snake.size() - 1].y, 1/*TODO*/, 0);
		ChangeObject(x, y, 1/*TODO*/, SNAKE);
		for (int j = 0; j < snakes.at(i).snake.size(); j++)
		{
			int id = snakes.at(i).snake.size() - j - 1;
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
}

void GamePlayEngine::ChangeSnakeDirection(int snake_id, char dir)
{
	// TODO params verification
	snakes.at(snake_id).snake[0].dir = dir;
}

FrameRenderingInput GamePlayEngine::GetFrameRenderingInput()
{
	return { physics, physw, physh, &snakes };
}
