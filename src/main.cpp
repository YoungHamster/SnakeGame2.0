#include <Windows.h>

#include "Renderer.h"
#include "GamePlayEngine.h"

static bool programRunning = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmd, int nCmdShow)
{
	Renderer renderer(hInstance, WindowProc, nCmdShow);
	GamePlayEngine gamePlayEngine(32, 18);
	gamePlayEngine.SpawnSnake(10, 10, 5, LEFT);
	gamePlayEngine.SpawnSnake(10, 15, 10, LEFT);
	gamePlayEngine.SpawnSnake(10, 20, 15, LEFT);
	MSG msg;
	msg.message = WM_NULL;
	while (programRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&msg);
		}
		else
		{
			renderer.RenderFrame(gamePlayEngine.GetFrameRenderingInput());
		}
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		programRunning = false;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}