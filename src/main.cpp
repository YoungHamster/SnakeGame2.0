#include <Windows.h>

#include "Renderer.h"
#include "GamePlayEngine.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmd, int nCmdShow)
{
	Renderer renderer(hInstance, WindowProc, nCmdShow);
	GamePlayEngine gamePlayEngine(32, 18);
	gamePlayEngine.SpawnSnake(10, 10, 10, LEFT);
	renderer.RenderFrame(gamePlayEngine.GetFrameRenderingInput());
	Sleep(3000);
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}