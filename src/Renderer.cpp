#include "Renderer.h"

Renderer::Renderer(HINSTANCE hInstance, LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam), int nCmdShow)
{
	/* Win api initialization */
	WNDCLASSEX windowclass;
	ZeroMemory(&windowclass, sizeof(WNDCLASSEX));
	windowclass.cbSize = sizeof(WNDCLASSEX);
	windowclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	windowclass.hInstance = hInstance;
	windowclass.lpfnWndProc = WindowProc;
	windowclass.lpszClassName = "MainWindow";
	windowclass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&windowclass);
	RECT rect = { 0, 0, SCREENWIDTH, SCREENHEIGTH };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND windowHandle = CreateWindowW(L"MainWindow", L"SnakeGame", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, 0);
	if (!windowHandle) return;

	this->windowHandle = windowHandle;

	ShowWindow(windowHandle, nCmdShow);

	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
	if (res != S_OK) rendererErrors.push_back(D2D1FactoryCreationError);
	GetClientRect(windowHandle, &rect);

	res = factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom), D2D1_PRESENT_OPTIONS_IMMEDIATELY),
		&rendertarget);
	if (res != S_OK) rendererErrors.push_back(HWNDRenderTargetCreationError);

	//Loading textures
	{
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadUp.png", &bitmaps[0])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadDown.png", &bitmaps[1])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadLeft.png", &bitmaps[2])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadRight.png", &bitmaps[3])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeBody.png", &bitmaps[4])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailUp.png", &bitmaps[5])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailDown.png", &bitmaps[6])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailLeft.png", &bitmaps[7])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailRight.png", &bitmaps[8])) rendererErrors.push_back(LoadingBitmapError);

		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadUp.png", &bitmaps[9])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadDown.png", &bitmaps[10])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadLeft.png", &bitmaps[11])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadRight.png", &bitmaps[12])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeBody.png", &bitmaps[13])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailUp.png", &bitmaps[14])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailDown.png", &bitmaps[15])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailLeft.png", &bitmaps[16])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailRight.png", &bitmaps[17])) rendererErrors.push_back(LoadingBitmapError);

		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadUp.png", &bitmaps[18])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadDown.png", &bitmaps[19])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadLeft.png", &bitmaps[20])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadRight.png", &bitmaps[21])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeBody.png", &bitmaps[22])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailUp.png", &bitmaps[23])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailDown.png", &bitmaps[24])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailLeft.png", &bitmaps[25])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailRight.png", &bitmaps[26])) rendererErrors.push_back(LoadingBitmapError);

		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadUp.png", &bitmaps[27])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadDown.png", &bitmaps[28])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadLeft.png", &bitmaps[29])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadRight.png", &bitmaps[30])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeBody.png", &bitmaps[31])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailUp.png", &bitmaps[32])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailDown.png", &bitmaps[33])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailLeft.png", &bitmaps[34])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailRight.png", &bitmaps[35])) rendererErrors.push_back(LoadingBitmapError);

		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\Red.png", &bitmaps[36])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\Apple.png", &bitmaps[37])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\Border.png", &bitmaps[38])) rendererErrors.push_back(LoadingBitmapError);

		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\A.png", &bitmaps[39])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\B.png", &bitmaps[40])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\C.png", &bitmaps[41])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\D.png", &bitmaps[42])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\E.png", &bitmaps[43])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\F.png", &bitmaps[44])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\G.png", &bitmaps[45])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\H.png", &bitmaps[46])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\I.png", &bitmaps[47])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\J.png", &bitmaps[48])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\K.png", &bitmaps[49])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\L.png", &bitmaps[50])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\M.png", &bitmaps[51])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\N.png", &bitmaps[52])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\O.png", &bitmaps[53])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\P.png", &bitmaps[54])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\Q.png", &bitmaps[55])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\R.png", &bitmaps[56])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\S.png", &bitmaps[57])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\T.png", &bitmaps[58])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\U.png", &bitmaps[59])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\V.png", &bitmaps[60])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\W.png", &bitmaps[61])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\X.png", &bitmaps[62])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\Y.png", &bitmaps[63])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\Z.png", &bitmaps[64])) rendererErrors.push_back(LoadingBitmapError);

		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\+.png", &bitmaps[65])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\-.png", &bitmaps[66])) rendererErrors.push_back(LoadingBitmapError);

		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\0.png", &bitmaps[67])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\1.png", &bitmaps[68])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\2.png", &bitmaps[69])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\3.png", &bitmaps[70])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\4.png", &bitmaps[71])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\5.png", &bitmaps[72])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\6.png", &bitmaps[73])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\7.png", &bitmaps[74])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\8.png", &bitmaps[75])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\9.png", &bitmaps[76])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\SPACE.png", &bitmaps[77])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\DOT.png", &bitmaps[78])) rendererErrors.push_back(LoadingBitmapError);
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\EXCLAMATION POINT.png", &bitmaps[79])) rendererErrors.push_back(LoadingBitmapError);
	}
}

Renderer::~Renderer()
{
	factory->Release();
	rendertarget->Release();
}

void Renderer::RenderFrame(FrameRenderingInput renderingInput)
{
	BeginDraw();
	ClearScreen();
	float pixelsperblockw = (float)rendertarget->GetPixelSize().width / (float)renderingInput.physicsWidth;
	float pixelsperblockh = (float)rendertarget->GetPixelSize().height / (float)renderingInput.physicsHeight;
	for (int i = 0; i < renderingInput.physicsHeight; i++)
	{
		for (int j = 0; j < renderingInput.physicsWidth; j++)
		{
			if (renderingInput.physics[i*renderingInput.physicsWidth + j].type == SNAKE)
			{
				RECT rect = { LONG(j * pixelsperblockw), LONG((renderingInput.physicsHeight - i) * pixelsperblockh), LONG((j + 1) * pixelsperblockw), LONG((renderingInput.physicsHeight - (i + 1)) * pixelsperblockh) };
				DrawBitmap(bitmaps[4], &rect, NULL, 1.0f);
			}
			if (renderingInput.physics[i*renderingInput.physicsWidth + j].type == BARRIER)
			{
				RECT rect = { LONG(j * pixelsperblockw), LONG((renderingInput.physicsHeight - i) * pixelsperblockh), LONG((j + 1) * pixelsperblockw), LONG((renderingInput.physicsHeight - (i + 1)) * pixelsperblockh) };
				DrawBitmap(bitmaps[38], &rect, NULL, 1.0f);
			}
		}
	}

	EndDraw();
}

//bool Renderer::Init(HWND windowHandle)
//{
//
//	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
//	if (res != S_OK) return false;
//	RECT rect;
//	GetClientRect(windowHandle, &rect);
//
//	res = factory->CreateHwndRenderTarget(
//		D2D1::RenderTargetProperties(),
//		D2D1::HwndRenderTargetProperties(windowHandle, D2D1::SizeU(rect.right, rect.bottom), D2D1_PRESENT_OPTIONS_IMMEDIATELY),
//		&rendertarget);
//	if (res != S_OK)
//	{
//		LastRendererError = CREATION_RENDER_TARGET_ERROR;
//		return false;
//	}
//
//	/*res = rendertarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0), &brush);
//	if (res != S_OK) 
//	{
//		return false;
//	}*/
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadUp.png", &bitmaps[0])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadDown.png", &bitmaps[1])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadLeft.png", &bitmaps[2])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeHeadRight.png", &bitmaps[3])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeBody.png", &bitmaps[4])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailUp.png", &bitmaps[5])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailDown.png", &bitmaps[6])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailLeft.png", &bitmaps[7])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\BlueSnakeTailRight.png", &bitmaps[8])) return false;
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadUp.png", &bitmaps[9])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadDown.png", &bitmaps[10])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadLeft.png", &bitmaps[11])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeHeadRight.png", &bitmaps[12])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeBody.png", &bitmaps[13])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailUp.png", &bitmaps[14])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailDown.png", &bitmaps[15])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailLeft.png", &bitmaps[16])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\GreenSnakeTailRight.png", &bitmaps[17])) return false;
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadUp.png", &bitmaps[18])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadDown.png", &bitmaps[19])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadLeft.png", &bitmaps[20])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeHeadRight.png", &bitmaps[21])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeBody.png", &bitmaps[22])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailUp.png", &bitmaps[23])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailDown.png", &bitmaps[24])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailLeft.png", &bitmaps[25])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\YellowSnakeTailRight.png", &bitmaps[26])) return false;
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadUp.png", &bitmaps[27])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadDown.png", &bitmaps[28])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadLeft.png", &bitmaps[29])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeHeadRight.png", &bitmaps[30])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeBody.png", &bitmaps[31])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailUp.png", &bitmaps[32])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailDown.png", &bitmaps[33])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailLeft.png", &bitmaps[34])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\VioletSnakeTailRight.png", &bitmaps[35])) return false;
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\Red.png", &bitmaps[36])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\Apple.png", &bitmaps[37])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\Border.png", &bitmaps[38])) return false;
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\A.png", &bitmaps[39])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\B.png", &bitmaps[40])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\C.png", &bitmaps[41])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\D.png", &bitmaps[42])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\E.png", &bitmaps[43])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\F.png", &bitmaps[44])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\G.png", &bitmaps[45])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\H.png", &bitmaps[46])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\I.png", &bitmaps[47])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\J.png", &bitmaps[48])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\K.png", &bitmaps[49])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\L.png", &bitmaps[50])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\M.png", &bitmaps[51])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\N.png", &bitmaps[52])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\O.png", &bitmaps[53])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\P.png", &bitmaps[54])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\Q.png", &bitmaps[55])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\R.png", &bitmaps[56])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\S.png", &bitmaps[57])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\T.png", &bitmaps[58])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\U.png", &bitmaps[59])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\V.png", &bitmaps[60])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\W.png", &bitmaps[61])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\X.png", &bitmaps[62])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\Y.png", &bitmaps[63])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\Z.png", &bitmaps[64])) return false;
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\+.png", &bitmaps[65])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\-.png", &bitmaps[66])) return false;
//
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\0.png", &bitmaps[67])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\1.png", &bitmaps[68])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\2.png", &bitmaps[69])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\3.png", &bitmaps[70])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\4.png", &bitmaps[71])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\5.png", &bitmaps[72])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\6.png", &bitmaps[73])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\7.png", &bitmaps[74])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\8.png", &bitmaps[75])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\9.png", &bitmaps[76])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\SPACE.png", &bitmaps[77])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\DOT.png", &bitmaps[78])) return false;
//	if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\ASCII\\EXCLAMATION POINT.png", &bitmaps[79])) return false;
//
//	for (int i = 0; i < BITMAPSNUMBER; i++)
//	{
//		bitmapsHWs[i] = bitmaps[i]->GetPixelSize().width;
//	}
//
//	return true;
//}

//void Renderer::RenderFrame(std::vector<PhysicalObject>& physics, std::vector<button>& buttons, bool menumode)
//{
//	float opacity;
//	float backgroundR;
//	float backgroundG;
//	float backgroundB;
//	if (menumode)
//	{
//		backgroundR = 0.5f;
//		backgroundG = 0.4f;
//		backgroundB = 0.4f;
//		opacity = 0.3f;
//	}
//	else
//	{
//		backgroundR = 0.4f;
//		backgroundG = 0.5f;
//		backgroundB = 0.4f;
//		opacity = 0.7f;
//	}
//
//	int renderWidth = this->rendertarget->GetPixelSize().width;
//	int renderHeigth = this->rendertarget->GetPixelSize().height;
//	float xFactor = renderWidth / GameFieldWidth;
//	float yFactor = renderHeigth / GameFieldHeigth;
//	BeginDraw();
//	ClearScreen(backgroundR, backgroundG, backgroundB);
//
//	RECT rect;
//	for (int i = 0; i < physics.size(); i++)
//	{
//		if (physics[i].type != DEAD_SNAKE && physics[i].type < 40 && physics[i].type > 0)
//		{
//			rect.left = physics[i].borders.min.x * xFactor;
//			rect.bottom = renderHeigth - physics[i].borders.min.y * yFactor;
//			rect.right = physics[i].borders.max.x * xFactor;
//			rect.top = renderHeigth - physics[i].borders.max.y * yFactor;
//			DrawBitmap(bitmaps[physics[i].type - 1], &rect, NULL, opacity);
//		}
//	}
//
//	for (int i = 0; i < buttons.size(); i++)
//	{
//		RenderButton(buttons[i]);
//	}
//	EndDraw();
//}

void Renderer::DrawBitmap(ID2D1Bitmap* bmp, RECT* rect, RECT* srcrect, float opacity)
{
	if (rect != NULL && srcrect != NULL)
	{
		rendertarget->DrawBitmap(
			bmp, // Bitmap
			D2D1::RectF((float)rect->left, (float)rect->top, (float)rect->right, (float)rect->bottom), // Destination rectangle
			opacity, // Opacity
			D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			D2D1::RectF((float)srcrect->left, (float)srcrect->top, (float)srcrect->right, (float)srcrect->bottom) // Source rectangle
		);
		return;
	}
	if (srcrect == NULL)
	{
		rendertarget->DrawBitmap(
			bmp, // Bitmap
			D2D1::RectF((float)rect->left, (float)rect->top, (float)rect->right, (float)rect->bottom), // Destination rectangle
			opacity, // Opacity
			D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			D2D1::RectF(0, 0, (float)bmp->GetPixelSize().width, (float)bmp->GetPixelSize().height) // Source rectangle
		);
		return;
	}
	if (rect == NULL)
	{
		rendertarget->DrawBitmap(
			bmp, // Bitmap
			D2D1::RectF(0, 0, (float)srcrect->right, (float)srcrect->bottom), // Destination rectangle
			opacity, // Opacity
			D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			D2D1::RectF((float)srcrect->left, (float)srcrect->top, (float)srcrect->right, (float)srcrect->bottom) // Source rectangle
		);
		return;
	}
	rendertarget->DrawBitmap(
		bmp, // Bitmap
		D2D1::RectF(0, 0, (float)bmp->GetPixelSize().width, (float)bmp->GetPixelSize().height), // Destination rectangle
		opacity, // Opacity
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		D2D1::RectF(0, 0, (float)bmp->GetPixelSize().width, (float)bmp->GetPixelSize().height) // Source rectangle
	);
}

void Renderer::ClearScreen()
{
	rendertarget->Clear(backgroundColor);
}

bool Renderer::LoadID2D1Bitmap(LPCWSTR filename, ID2D1Bitmap **ppBitmap)
{
	*ppBitmap = NULL;
	HRESULT hr;

	IWICImagingFactory *wicFactory = NULL;
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		//CLSID_WICPngDecoder,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&wicFactory);

	if (hr != S_OK) return false;

	// Create a decoder
	IWICBitmapDecoder *wicDecoder = NULL;
	hr = wicFactory->CreateDecoderFromFilename(
		filename,		// The file name
		NULL,			// No preffered vendor
		GENERIC_READ,	// Reading the file, not writing
		WICDecodeMetadataCacheOnLoad, // Cache on load*
		&wicDecoder);	// The decoder we're making

	if (hr != S_OK) return false;

						// Read a frame from the image
	IWICBitmapFrameDecode* wicFrame = NULL;
	hr = wicDecoder->GetFrame(0, &wicFrame);

	if (hr != S_OK) return false;

	// Create a converter
	IWICFormatConverter *wicConverter = NULL;
	hr = wicFactory->CreateFormatConverter(&wicConverter);

	if (hr != S_OK) return false;

	// Setup converter!
	hr = wicConverter->Initialize(
		wicFrame,	// Frame
		GUID_WICPixelFormat32bppPBGRA, // Pixel format
		WICBitmapDitherTypeNone,	// Irrelevant
		NULL,		// No palette needed, irrelevant
		0.0,		// Alpha transparency, irrelevant
		WICBitmapPaletteTypeCustom); // Irrelevant

	if (hr != S_OK) return false;

									 // Use a converter to create an D2D1Bitmap
	hr = rendertarget->CreateBitmapFromWicBitmap(
		wicConverter, // Converter
		NULL,		  // D2D1_BITMAP_PROPERTIES
		ppBitmap);		  // Destination D2D1 bitmap

	if (hr != S_OK) return false;

	if (wicFactory) wicFactory->Release();
	if (wicDecoder) wicDecoder->Release();
	if (wicConverter) wicConverter->Release();
	if (wicFrame) wicFrame->Release();

	return true;
}

void Renderer::DrawTextOnRend(const wchar_t* text, int Size, int minX, int minY)
{
	int DrawOffset = 0;
	int CurrentCharacterInBitmapsArrayID = 77;
	RECT rect;
	rect.bottom = minY;
	for (int i = 0; i < wstrlen(text); i++)
	{
		if (text[i] >= L'A' && text[i] <= L'Z')	{ CurrentCharacterInBitmapsArrayID = text[i] - 26; }
		if (text[i] == L'+') { CurrentCharacterInBitmapsArrayID = 65; }
		if (text[i] == L'-') { CurrentCharacterInBitmapsArrayID = 66; }
		if (text[i] >= L'0' && text[i] <= L'9')	{ CurrentCharacterInBitmapsArrayID = text[i] + 19; }
		if (text[i] == L' ') { CurrentCharacterInBitmapsArrayID = 77; }
		if (text[i] == L'.') { CurrentCharacterInBitmapsArrayID = 78; }
		if (text[i] == L'!') { CurrentCharacterInBitmapsArrayID = 79; }
		rect.left = minX + DrawOffset;
		rect.right = rect.left + Size * bitmaps[CurrentCharacterInBitmapsArrayID]->GetPixelSize().width;
		rect.top = rect.bottom + Size * bitmaps[CurrentCharacterInBitmapsArrayID]->GetPixelSize().height;
		DrawBitmap(bitmaps[CurrentCharacterInBitmapsArrayID], &rect, NULL, 0.9f);
		DrawOffset = DrawOffset + rect.right - rect.left + Size;
	}
}

//void Renderer::RenderButton(button button)
//{
//	const wchar_t* text;
//	if (button.changableText != NULL)
//	{
//		text = button.changableText->c_str();
//	}
//	else
//	{
//		text = button.constantText.c_str();
//	}
//	if (!button.Enlarged)
//	{
//		if (button.Centered)
//		{
//			DrawTextOnRend(text, button.Size, (rendertarget->GetPixelSize().width / 2) - CountTextWidth(text, button.Size) / 2, button.ClickBox.min.y);
//		}
//		else
//		{
//			DrawTextOnRend(text, button.Size, button.ClickBox.min.x, button.ClickBox.min.y);
//		}
//	}
//	else
//	{
//		if (button.Centered)
//		{
//			DrawTextOnRend(text, button.Size + 1, (rendertarget->GetPixelSize().width / 2) - CountTextWidth(text, button.Size + 1) / 2, button.ClickBox.min.y);
//		}
//		else
//		{
//			DrawTextOnRend(text, button.Size + 1, button.ClickBox.min.x, button.ClickBox.min.y);
//		}
//	}
//}

int Renderer::CountTextWidth(const wchar_t* text, int Size)
{
	int Width = 0;
	int CurrentCharacterInBitmapsArrayID = 77;
	int currentW = 0;
	for (int i = 0; i < wstrlen(text); i++)
	{
		if (text[i] >= L'A' && text[i] <= L'Z') { CurrentCharacterInBitmapsArrayID = text[i] - 26; }
		if (text[i] == L'+') { CurrentCharacterInBitmapsArrayID = 65; }
		if (text[i] == L'-') { CurrentCharacterInBitmapsArrayID = 66; }
		if (text[i] >= L'0' && text[i] <= L'9') { CurrentCharacterInBitmapsArrayID = text[i] + 19; }
		if (text[i] == L' ') { CurrentCharacterInBitmapsArrayID = 77; }
		if (text[i] == L'.') { CurrentCharacterInBitmapsArrayID = 78; }
		if (text[i] == L'!') { CurrentCharacterInBitmapsArrayID = 79; }
		//currentW = bitmapsHWs[CurrentCharacterInBitmapsArrayID] * Size;//bitmaps[CurrentCharacterInBitmapsArrayID]->GetPixelSize().width;
		Width = Width + currentW + Size;
	}
	return Width;
}

RendererErrors Renderer::GetLastRendererError()
{
	return rendererErrors[rendererErrors.back()];
}

int wstrlen(const wchar_t* str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		++i;
	}
	return i;
}

wchar_t* wstrcat(const wchar_t *str1, const wchar_t *str2)
{
	wchar_t *str = new wchar_t[(wstrlen(str1) + wstrlen(str2) + 1)];
	for (int i = 0; i < wstrlen(str); i++)
	{
		if (i < wstrlen(str1))
		{
			str[i] = str1[i];
		}
		else
		{
			str[i] = str2[i - wstrlen(str1)];
		}
	}
	return str;
}