#pragma once

#include <vector>

// Physical objects possible types
#define NOTHING 0
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