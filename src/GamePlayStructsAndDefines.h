#pragma once

#include "DynamicArray.h"

// Physical objects possible types
#define NOTHING 0
#define BARRIER 1
#define SNAKE 2
#define APPLE 3

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#define SNAKE_FOV_WIDTH 9
#define SNAKE_FOV_HEIGHT 9
enum AITypes
{
	realPlayer,
	ai
};

struct PhysicalObject
{
	int type = 0;
};

struct SnakeBlock
{
	int x;
	int y;
	char dir;
};

struct AIWeight
{
	short up;
	short down;
	short left;
	short right;

	AIWeight(short up, short down, short left, short right)
		:up(up), down(down), left(left), right(right) {}

	AIWeight operator+(AIWeight operand)
	{
		return { this->up + operand.up, this->down + operand.down,
			this->left + operand.left, this->right + operand.right };
	}
	char GetDirection()
	{
		if (up >= down && up >= left && up >= right)
		{
			return UP;
		}
		if (down >= up && down >= left && down >= right)
		{
			return DOWN;
		}
		if (left >= up && left >= down && left >= right)
		{
			return LEFT;
		}
		return RIGHT;
	}
};

struct Snake
{
	int texture = -1;
	char newdir = 0;
	DynamicArray snake;
	AITypes aiType;
	bool weightsWereChanged = false;
	AIWeight foodWeights[SNAKE_FOV_WIDTH][SNAKE_FOV_HEIGHT] = {
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, 0, 1, 0),   AIWeight(1, 0, 1, 0),   AIWeight(1, 0, 1, 0),   AIWeight(1, -1, 0, 0), AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1) },
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, 0, 1, 0),   AIWeight(1, 0, 1, 0),   AIWeight(1, -1, 1, -1), AIWeight(2, -2, 0, 0), AIWeight(1, -1, -1, 1), AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1) },
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, 0, 1, 0),   AIWeight(1, -1, 1, -1), AIWeight(2, -2, 2, -2), AIWeight(3, -3, 0, 0), AIWeight(2, -2, -2, 2), AIWeight(1, -1, -1, 1), AIWeight(1, 0, 0, 1),   AIWeight(1, 0, 0, 1) },
	{ AIWeight(1, 0, 1, 0),  AIWeight(1, -1, 1, -1), AIWeight(2, -2, 2, -2), AIWeight(3, -3, 3, -3), AIWeight(4, -4, 0, 0), AIWeight(3, -3, -3, 3), AIWeight(2, -2, -2, 2), AIWeight(1, -1, -1, 1), AIWeight(1, 0, 0, 1) },
	{ AIWeight(0, 0, 1, -1), AIWeight(0, 0, 2, -2),  AIWeight(0, 0, 3, -3),  AIWeight(0, 0, 4, -4),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -4, 4),  AIWeight(0, 0, -3, 3),  AIWeight(0, 0, -2, 2),  AIWeight(0, 0, -1, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(-1, 1, 1, -1), AIWeight(-2, 2, 2, -2), AIWeight(-3, 3, 3, -3), AIWeight(-4, 4, 0, 0), AIWeight(-3, 3, -3, 3), AIWeight(-2, 2, -2, 2), AIWeight(-1, 1, -1, 1), AIWeight(0, 1, 0, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(0, 1, 1, 0),   AIWeight(-1, 1, 1, -1), AIWeight(-2, 2, 2, -2), AIWeight(-3, 3, 0, 0), AIWeight(-2, 2, -2, 2), AIWeight(-1, 1, -1, 1), AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(0, 1, 1, 0),   AIWeight(0, 1, 1, 0),   AIWeight(-1, 1, 1, -1), AIWeight(-2, 2, 0, 0), AIWeight(-1, 1, -1, 1), AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1) },
	{ AIWeight(0, 1, 1, 0),  AIWeight(0, 1, 1, 0),   AIWeight(0, 1, 1, 0),   AIWeight(0, 1, 1, 0),   AIWeight(-1, 1, 0, 0), AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1),   AIWeight(0, 1, 0, 1) }
	};
	AIWeight obstacleWeights[SNAKE_FOV_WIDTH][SNAKE_FOV_HEIGHT] = {
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(-1, 1, 1, 1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -1, 0), AIWeight(-2, 2, 2, 2), AIWeight(0, 0, 0, -1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -2, 0), AIWeight(-3, 3, 3, 3), AIWeight(0, 0, 0, -2), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(3, 3, -3, 3), AIWeight(-4, 4, 4, 4), AIWeight(3, 3, 3, -3), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(1, 1, -1, 1), AIWeight(2, 2, -2, 2), AIWeight(3, 3, -3, 3), AIWeight(4, 4, -4, 4), AIWeight(0, 0, 0, 0),  AIWeight(4, 4, 4, -4), AIWeight(3, 3, 3, -3), AIWeight(2, 2, 2, -2), AIWeight(1, 1, 1, -1) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(3, 3, -3, 3), AIWeight(4, -4, 4, 4), AIWeight(3, 3, 3, -3), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -2, 0), AIWeight(3, -3, 3, 3), AIWeight(0, 0, 0, -2), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, -1, 0), AIWeight(2, -2, 2, 2), AIWeight(0, 0, 0, -1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) },
	{ AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(1, -1, 1, 1), AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0),  AIWeight(0, 0, 0, 0) }
	};
	Snake():snake(sizeof(SnakeBlock)){}
};