#pragma once

#include <Windows.h>
#include <d2d1.h>
#include <vector>
#include <wincodec.h>
#include <string>

#include "PhysicalObject.h"
#include "FrameRenderingInput.h"

#define RENDERER

#define BITMAPSNUMBER 80
#define SCREENWIDTH 1280
#define SCREENHEIGTH 720
#define GAMEFIELDHEIGTH (SCREENHEIGTH / 30)
#define GAMEFIELDWIDTH (SCREENWIDTH / 30)

enum RendererErrors
{
	LoadingBitmapError,
	HWNDRenderTargetCreationError,
	D2D1FactoryCreationError
};

class Renderer
{
private:
	ID2D1Factory * factory;
	ID2D1HwndRenderTarget* rendertarget;

	// 39 GameTextures + 41 ASCII Symbols
	ID2D1Bitmap* bitmaps[BITMAPSNUMBER];

	std::vector<RendererErrors> rendererErrors;

	D2D1::ColorF backgroundColor = { 0.0f, 0.0f, 0.0f };

	void BeginDraw() { rendertarget->BeginDraw(); }
	void EndDraw() { rendertarget->EndDraw(); }
	void ClearScreen();
	bool LoadID2D1Bitmap(LPCWSTR filename, ID2D1Bitmap **ppBitmap);
	void DrawBitmap(ID2D1Bitmap* bmp, RECT* rect, RECT* srcrect, float opacity);

	void DrawTextOnRend(const wchar_t* text, int Size, int minX, int minY);
	//void RenderButton(button button);

public:
	
	Renderer(HINSTANCE hInstance, LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam), int nCmdShow);
	~Renderer();
	//bool Init(HWND windowhandle); Deprecated
	//void RenderFrame(std::vector<PhysicalObject>& physics, std::vector<button>& buttons, bool menumode); Deprecated
	void RenderFrame(FrameRenderingInput renderingInput);

	/* Counts text width in pixels */
	int CountTextWidth(const wchar_t* text, int Size);

	D2D1_SIZE_U GetRenderTargetSize();

	RendererErrors GetLastRendererError();

};

int wstrlen(const wchar_t* str);

wchar_t* wstrcat(const wchar_t *str1, const wchar_t *str2);