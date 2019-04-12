#pragma once

#include <vector>
#include "DynamicArray.h"

struct FrameRenderingInput
{
	PhysicalObject *physics;
	int physicsWidth;
	int physicsHeight;
	DynamicArray *snakes = nullptr;
	Snake* GetSnake(int snake_id)
	{
		return (Snake*)snakes->At(snake_id);
	}
	SnakeBlock* GetSnakeBlock(int snake_id, int block_id)
	{
		return (SnakeBlock*)GetSnake(snake_id)->snake[block_id];
	}
};