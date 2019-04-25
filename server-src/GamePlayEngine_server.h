#pragma once

#include "src//GamePlayEngine.h"

class GamePlayEngine_server : public GamePlayEngine
{
private:
public:
	GamePlayEngine_server(int physw, int physh);
	~GamePlayEngine_server();
};