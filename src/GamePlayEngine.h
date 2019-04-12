#pragma once

#include "GamePlayStructsAndDefines.h"
#include "FrameRenderingInput.h"
#include "DynamicArray.h"

class GamePlayEngine
{
private:
	int physw = 0;
	int physh = 0;
	PhysicalObject *physics;
	int number_of_snakes;
	std::vector<Snake> snakes;

	void ChangeObject(int x, int y, int type);
	int GetObjType(int x, int y);
	void DespawnSnake(int snake_id);
	void FeedSnake(int snake_id);
	void ShortenSnake(int snake_id);
	void HandleSnakeAI(int snake_id);
	void SplitSnake(int snake_id, bool evolutionaryMode);
public:
	GamePlayEngine(int physw, int physh);
	~GamePlayEngine();
	int SpawnSnake(int size, int headx, int heady, char headdir, AITypes aiType);
	void SplitSnakes(int minLenghtToSplit, bool evolutionaryMode);
	void SpawnApple();
	void MoveSnakes();
	void ChangeSnakeDirection(int snake_id, char dir);
	FrameRenderingInput GetFrameRenderingInput();
	int GetNumberOfSnakes();
};