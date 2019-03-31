#pragma once

#include "GamePlayStructsAndDefines.h"
#include "FrameRenderingInput.h"

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
	FrameRenderingInput GetFrameRenderingInput();
	int GetNumberOfSnakes();
};