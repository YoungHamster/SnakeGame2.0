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

	windowHandle = CreateWindowW(L"MainWindow", L"SnakeGame", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, 0);
	if (!windowHandle) return;

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
		if (!LoadID2D1Bitmap(L"res\\textures\\NewTextures\\WhiteGlow.png", &bitmaps[80])) rendererErrors.push_back(LoadingBitmapError);
	}
}

Renderer::~Renderer()
{
	factory->Release();
	rendertarget->Release();
}

void Renderer::BeginDraw() { rendertarget->BeginDraw(); }

void Renderer::EndDraw() { rendertarget->EndDraw(); }

void Renderer::RenderFrame(FrameRenderingInput renderingInput)
{
	float pixelsperblockw = (float)rendertarget->GetPixelSize().width / (float)renderingInput.physicsWidth;
	float pixelsperblockh = (float)rendertarget->GetPixelSize().height / (float)renderingInput.physicsHeight;
	BeginDraw();
	ClearScreen();
	RECT rect;
	
	for (int i = 0; i < renderingInput.physicsHeight; i++)
	{
		for (int j = 0; j < renderingInput.physicsWidth; j++)
		{
			if (renderingInput.physics[i*renderingInput.physicsWidth + j].type == APPLE ||
				renderingInput.physics[i*renderingInput.physicsWidth + j].type == BARRIER)
			{
				rect = { LONG(j * pixelsperblockw),
					LONG((renderingInput.physicsHeight - i) * pixelsperblockh),
					LONG((j + 1) * pixelsperblockw),
					LONG((renderingInput.physicsHeight - (i + 1)) * pixelsperblockh) };
				switch (renderingInput.physics[i*renderingInput.physicsWidth + j].type)
				{
				case APPLE: 
					DrawBitmap(bitmaps[37], rect, 1.0f); // 37 is apple texture
					break;
				case BARRIER:
					DrawBitmap(bitmaps[38], rect, 1.0f); // 38 is barrier texture
					break;
				}
			}
		}
	}

	int bitmapId = 0;
	for (int i = 0; i < renderingInput.numberOfSnakes; i++)
	{
		for (int j = 0; j < renderingInput.snakes[i].bodySize; j++)
			
		{
			rect = { LONG(renderingInput.snakes[i].body[j].x * pixelsperblockw),
				LONG((renderingInput.physicsHeight - renderingInput.snakes[i].body[j].y) * pixelsperblockh),
				LONG((renderingInput.snakes[i].body[j].x + 1) * pixelsperblockw),
				LONG((renderingInput.physicsHeight - (renderingInput.snakes[i].body[j].y + 1)) * pixelsperblockh) };
			if (renderingInput.snakes[i].texture == -1)
			{
				bitmapId = i % 4 * 9;
				renderingInput.snakes[i].texture = bitmapId;
			}
			else
			{
				bitmapId = renderingInput.snakes[i].texture;
			}
			if (j != 0 && j != renderingInput.snakes[i].bodySize - 1)
			{
				bitmapId += 4;
			}
			else
			{
				switch (renderingInput.snakes[i].body[j].dir)
				{
				case DOWN: bitmapId += 1; break;
				case LEFT: bitmapId += 2; break;
				case RIGHT: bitmapId += 3; break;
				}
				if (j != 0)
				{
					bitmapId += 5;
				}
			}
			DrawBitmap(bitmaps[bitmapId], rect, 0.75f);

			// glowing effect for snakes
			rect.left -= LONG(pixelsperblockw * 2);
			rect.top -= LONG(pixelsperblockh * 2);
			rect.right += LONG(pixelsperblockw * 2);
			rect.bottom += LONG(pixelsperblockh * 2);
			DrawBitmap(bitmaps[80], rect, 0.1f);
		}
	}
	EndDraw();
}

void Renderer::DragCamera(float x, float y)
{
	rendertarget->SetTransform(D2D1::Matrix3x2F::Translation(x, y));
}

HWND Renderer::GetWindowHandle() { return windowHandle; }

void Renderer::DrawBitmap(ID2D1Bitmap* bmp, RECT rect, float opacity)
{
	rendertarget->DrawBitmap(
		bmp, // Bitmap
		D2D1::RectF((float)rect.left, (float)rect.top, (float)rect.right, (float)rect.bottom), // Destination rectangle
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

RendererErrors Renderer::GetError()
{
	if (rendererErrors.size() == 0)
	{
		return NoErrors;
	}
	else
	{
		RendererErrors error = rendererErrors[0];
		rendererErrors.erase(rendererErrors.begin());
		return error;
	}
}