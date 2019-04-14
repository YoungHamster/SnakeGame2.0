#pragma once

struct FrameRenderingInput
{
	PhysicalObject *physics = nullptr;
	int physicsWidth;
	int physicsHeight;
	int numberOfSnakes;
	Snake *snakes = nullptr;
};