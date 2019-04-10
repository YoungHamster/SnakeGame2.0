#pragma once

#include <Windows.h>
#include <string>

#include <d2d1.h>
#include <wincodec.h>

#include "GamePlayStructsAndDefines.h"
#include "FrameRenderingInput.h"
#include "CyclicBuffer.h"

#define BITMAPSNUMBER 81
#define SCREENWIDTH 1280
#define SCREENHEIGTH 720

enum RendererErrors
{
	NoErrors,
	LoadingBitmapError,
	HWNDRenderTargetCreationError,
	D2D1FactoryCreationError
};

struct Camera
{
	float x = 0.0f;
	float y = 0.0f;
	float scale = 0.0f;
};

class Renderer
{
private:
	HWND windowHandle;
	ID2D1Factory * factory;
	ID2D1HwndRenderTarget* rendertarget;
	// 39 GameTextures + 41 ASCII Symbols
	ID2D1Bitmap* bitmaps[BITMAPSNUMBER];
	std::vector<RendererErrors> rendererErrors;
	D2D1::ColorF backgroundColor = { 0.8f, 0.5f, 0.5f };
	CyclicBuffer renderingInputHistory;
	CyclicBuffer translationHistory;
	Camera camera;

	void BeginDraw();
	void EndDraw();
	void ClearScreen();
	bool LoadID2D1Bitmap(LPCWSTR filename, ID2D1Bitmap **ppBitmap);
	void DrawBitmap(ID2D1Bitmap* bmp, RECT* rect, RECT* srcrect, float opacity);

public:
	
	Renderer(HINSTANCE hInstance, LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam), int nCmdShow);
	~Renderer();
	void RenderFrame(FrameRenderingInput renderingInput);
	void DragCamera(float deltax, float deltay);
	HWND GetWindowHandle();
	RendererErrors GetError();

};