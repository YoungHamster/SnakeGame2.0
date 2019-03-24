#pragma once
#include <vector>

#define PhysW 1280
#define PhysH 720
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define BARRIER 1
#define SNAKE 2
#define APPLE 3

struct PhysicalObject
{
	int texture = 0;
	int type = 0;
};

struct SnakeBlock
{
	int x;
	int y;
	char dir;
};

struct Snake
{
	std::vector<SnakeBlock> snake;
};

class GamePlayEngine
{
private:
	int physw = 0;
	int physh = 0;
	PhysicalObject *physics;
	int number_of_snakes;
	std::vector<Snake> snakes;
public:
	GamePlayEngine(int physw, int physh);
	~GamePlayEngine();
	void ChangeObject(int x, int y, int texture, int type);
	int GetObjType(int x, int y);
	int SpawnSnake(int size, int headx, int heady, char headdir);
	void DespawnSnake(int snake_id);
	void FeedSnake(int snake_id);
	void ShortenSnake(int snake_id);
	void MoveSnakes();
	void ChangeSnakeDirection(int snake_id, char dir);
};