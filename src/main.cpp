#include <Windows.h>

#include "Renderer.h"
#include "GamePlayEngine.h"

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
				case VK_RETURN:
					gamePlayEngine.MoveSnakes();
					break;
				case VK_F1:
					gamePlayEngine.SpawnSnake(3, 10, (newSnakey % 16) + 1, LEFT);
					newSnakey += 1;
					break;
				}
				inputBuffer.erase(inputBuffer.begin());
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