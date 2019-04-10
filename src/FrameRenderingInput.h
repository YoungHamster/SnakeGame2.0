#pragma once

#include <vector>

struct FrameRenderingInput
{
	PhysicalObject *physics;
	int physicsWidth;
	int physicsHeight;
	std::vector<Snake> *snakes = nullptr;
};