#include <Windows.h>
#include <chrono>

#include "Renderer.h"
#include "GamePlayEngine.h"

#define W 0x57
#define A 0x41
#define S 0x53
#define D 0x44
#define COUNTEXECUTIONTIME(x)\
auto starttime = std::chrono::high_resolution_clock::now();\
 x;\
auto endtime = std::chrono::high_resolution_clock::now();\
if(std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count() > 0)\
{\
	AllocConsole(); \
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE); \
	std::string delta(std::to_string(__LINE__) + ' ' + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count()) + " ms\n"); \
	WriteConsole(out, delta.c_str(), delta.size(), NULL, NULL);\
}

static bool programRunning = true;

static std::vector<WPARAM> inputBuffer;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ProcessInput();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmd, int nCmdShow)
{
	Renderer renderer(hInstance, WindowProc, nCmdShow);
	GamePlayEngine gamePlayEngine(32, 18);
	MSG msg;
	msg.message = WM_NULL;

	gamePlayEngine.SpawnSnake(3, 10, 1, LEFT);
	gamePlayEngine.SpawnSnake(3, 8, 1, RIGHT);

	int newSnakey = 1;
	int lastTickTime = 0;
	bool gameRunning = false;
	int appleSpawnCounter = 5;
	while (programRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);
		}
		else
		{
			if (gamePlayEngine.GetNumberOfSnakes() == 0)
			{
				//gamePlayEngine.SpawnSnake(3, 32 / 2 - 2, 18 / 2, LEFT);
			}
			if (inputBuffer.size() > 0)
			{
				switch (inputBuffer[0])
				{
				case W: gamePlayEngine.ChangeSnakeDirection(0, UP); break;
				case A: gamePlayEngine.ChangeSnakeDirection(0, LEFT); break;
				case S: gamePlayEngine.ChangeSnakeDirection(0, DOWN); break;
				case D: gamePlayEngine.ChangeSnakeDirection(0, RIGHT); break;
				case VK_RETURN:
					gameRunning = !gameRunning;
					break;
				case VK_F1:
					gamePlayEngine.SpawnSnake(3, 10, (newSnakey % 16) + 1, LEFT);
					newSnakey += 1;
					break;
				}
				inputBuffer.erase(inputBuffer.begin());
			}
			if (gameRunning && clock() - lastTickTime > 100)
			{
				gamePlayEngine.MoveSnakes();
				lastTickTime = clock();
				appleSpawnCounter += 1;
				if (appleSpawnCounter >= 5)
				{
					appleSpawnCounter = 0;
					gamePlayEngine.SpawnApple(rand() % 30 + 1, rand() % 16 + 1);
				}
			}
			renderer.RenderFrame(gamePlayEngine.GetFrameRenderingInput());
		}
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		programRunning = false;
		PostQuitMessage(0);
		return 0;
		break;
	case WM_KEYDOWN:
		inputBuffer.push_back(wParam);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}