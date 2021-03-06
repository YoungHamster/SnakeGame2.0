#include <Windows.h>
#include <Windowsx.h>
#include <chrono>

#include "Renderer.h"
#include "GamePlayEngine.h"
#include "Console.h"

#define W 0x57
#define A 0x41
#define S 0x53
#define D 0x44
#define COUNTEXECUTIONTIME(x)\
auto starttime = std::chrono::high_resolution_clock::now();\
 x;\
auto endtime = std::chrono::high_resolution_clock::now();\
if(std::chrono::duration_cast<std::chrono::nanoseconds>(endtime - starttime).count() > 0)\
{\
	AllocConsole(); \
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE); \
	std::string delta(std::to_string(__LINE__) + ' ' + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count()) + " ms\n"); \
	WriteConsole(out, delta.c_str(), delta.size(), NULL, NULL);\
}

struct MouseDragInput
{
	bool leftButtonPressed;
	POINT startPoint; // point where cursor was, when user pressed mouse button
	POINT currentPos;
};

static bool programRunning = true;

static MouseDragInput mouseDragInput;
static std::vector<WPARAM> inputBuffer;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ProcessInput();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmd, int nCmdShow)
{
	CreateConsole();
	std::cout << "Hello world!" << std::endl;

	Renderer renderer(hInstance, WindowProc, nCmdShow);
	int physw = 48;
	int physh = 27;
	GamePlayEngine gamePlayEngine(physw, physh);
	MSG msg;
	msg.message = WM_NULL;

	int newSnakey = 0;
	int lastTickTime = 0;
	bool gameRunning = true;
	while (programRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);
		}
		else
		{
			if (inputBuffer.size() > 0)
			{
				switch (inputBuffer[0])
				{
				case W: gamePlayEngine.ChangeSnakeDirection(0, UP); break;
				case A: gamePlayEngine.ChangeSnakeDirection(0, LEFT); break;
				case S: gamePlayEngine.ChangeSnakeDirection(0, DOWN); break;
				case D: gamePlayEngine.ChangeSnakeDirection(0, RIGHT); break;
				case VK_ESCAPE: gameRunning = !gameRunning;	break;
				case VK_F1:
					gamePlayEngine.SpawnSnake(3, physw / 2, (newSnakey % (physh - 2)) + 1, LEFT, ai);
					newSnakey += 1;
					break;
				}
				inputBuffer.erase(inputBuffer.begin());
			}
			if (gameRunning && clock() - lastTickTime >= 80)
			{
				lastTickTime = clock();
				if (!gamePlayEngine.IsAnyPlayerAlive())
				{
					lastTickTime += 1000;
					gamePlayEngine.KillAllSnakes();
					gamePlayEngine.SpawnSnake(3, 30, 10, LEFT, realPlayer);
					gamePlayEngine.SpawnSnake(3, 30, 13, LEFT, ai);
					gamePlayEngine.SpawnSnake(3, 30, 16, LEFT, ai);
					gamePlayEngine.SpawnSnake(3, 30, 19, LEFT, ai);
				}
				gamePlayEngine.MoveSnakes();
				gamePlayEngine.SpawnApple();
			}
			COUNTEXECUTIONTIME(renderer.RenderFrame(gamePlayEngine.GetFrameRenderingInput()));
			Sleep(2);
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
	case WM_LBUTTONDOWN:
		mouseDragInput.startPoint = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		mouseDragInput.leftButtonPressed = true;
		break;
	case WM_LBUTTONUP:
		mouseDragInput.leftButtonPressed = false;
		break;
	case WM_MOUSEMOVE:
		mouseDragInput.currentPos = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}