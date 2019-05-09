#pragma once

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
	char type = 0;
};

struct SnakeBlock
{
	short x;
	short y;
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
	AITypes aiType;
	int texture = -1;
	char newdir = 0;
	SnakeBlock* body = nullptr;
	int bodySize = 0;
	/* If body of snake becomes bigger, then supposed, 
	snakes manager allocates new block of memory specifically for this snake and sets this parameter to false */
	bool bodyAllocatedInCommonBuffer = true;
	int sizeAllocatedForBody = 0;
};