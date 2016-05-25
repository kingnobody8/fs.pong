#include "Game.h"

Game::Game(void)
{
	bRunning = TRUE;
	hWnd = NULL;
	ZeroMemory(keys, sizeof(keys));

	pD2DFactory = NULL;
	pRT = NULL;
	pBrush = NULL;
	pDWFactory = NULL;
	pTF = NULL;

	//set the bmps to null
	backGround_bmp = NULL;
	blocks_bmp = NULL;
	mario_bmp = NULL;
	luigi_bmp = NULL;
	shell_bmp = NULL;
	menu_bmp = NULL;


	// Zero out the frames per second variables:
	dwFrames = 0;
	dwCurrentTime = 0;
	dwLastUpdateTime = 0;
	dwElapsedTime = 0;
	szFPS = _T("");

	gamestate = MENU;
}

Game::~Game(void)
{
}

void Game::Startup(void)
{
	//TODO: Create Back Buffer

	Player1 = new CXBOXController(1);
	Player2 = new CXBOXController(2);

	mario.score = luigi.score = 0;

	// Initialize DirectX.
	HRESULT hr = CreateGraphics(hWnd);
	if (FAILED(hr))
	{
		return; // Return -1 to abort Window Creation.
	}

	//TODO: Load Bitmaps/Brushes/Pens/Fonts

	fullScreen = false;
	//TODO: Initialize Game Components
	openMap("default_Map.txt");

	POINT p;
	p.x = 80;
	p.y = 128;
	SIZE s;
	s.cx = 32;
	s.cy = 32;
	SIZE vel;
	vel.cx = vel.cy = 0;

	mario.setStats(p, s, LEFT, vel, 8);
	p.x = pRT->GetSize().width - 128;
	luigi.setStats(p, s, LEFT, vel, 8);

	POINT point;
	SIZE size;
	point.x = pRT->GetSize().width / 2 - 8;
	point.y = pRT->GetSize().height - 664;
	size.cx = size.cy = 16;
	shell.setStats(point, size);

}

void Game::Shutdown(void)
{
	//TODO: Unload Bitmaps/Brushes/Pens/Fonts

	//TODO: Destroy Back Buffer

	delete(Player1);
	delete(Player2);

	// Shutdown DirectX.
	DestroyGraphics();
}

void Game::Input(void)
{
	switch(gamestate)
	{
	case MENU:
		if(keys[VK_SPACE] & 0x80 || keys[VK_RETURN] & 0x80)
		{
			gamestate = PLAY;
			reset();
		}
		break;
	case PLAY:
		static bool spacePressed;
		//TODO: Read User Input
		if (keys[VK_ESCAPE] & 0x80)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		if(Player1->IsConnected())
		{
			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{ mario.handleButtons(UP, true, &luigi, &shell); }
			else
			{ mario.handleButtons(UP, false, &luigi, &shell); }
			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{ mario.handleButtons(SPIN, true, &luigi, &shell); }
			else
			{ mario.handleButtons(SPIN, false, &luigi, &shell); }
			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{ mario.handleButtons(GRAB, true, &luigi, &shell); }
			else
			{ mario.handleButtons(GRAB, false, &luigi, &shell); }
			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{ mario.handleButtons(LEFT_CK, true, &luigi, &shell); }
			else
			{ mario.handleButtons(LEFT_CK, false, &luigi, &shell);}
			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{ mario.handleButtons(RIGHT_CK, true, &luigi, &shell); }
			else
			{ mario.handleButtons(RIGHT_CK, false, &luigi, &shell);}
			if(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{ PostMessage(hWnd, WM_CLOSE, 0, 0); }
		}
		else
		{
			// Check if the W Key is pressed:
			if(keys['G'] & 0x80)
				mario.handleButtons(GRAB, true, &luigi, &shell);
			else
				mario.handleButtons(GRAB, false, &luigi, &shell);
			if(keys['H'] & 0x80)
				mario.handleButtons(SPIN, true, &luigi, &shell);
			else
				mario.handleButtons(SPIN, false, &luigi, &shell);
			if (keys['W'] & 0x80)
				mario.handleButtons(UP, true, &luigi, &shell);
			else
				mario.handleButtons(UP, false, &luigi, &shell);
			if(keys['A'] & 0x80)
				mario.handleButtons(LEFT_CK, true, &luigi, &shell);
			else
				mario.handleButtons(LEFT_CK, false, &luigi, &shell);
			if(keys['S'] & 0x80)
				mario.handleButtons(BLOCK, true, &luigi, &shell);
			else
				mario.handleButtons(BLOCK, false, &luigi, &shell);
			if(keys['D'] & 0x80)
				mario.handleButtons(RIGHT_CK, true, &luigi, &shell);
			else
				mario.handleButtons(RIGHT_CK, false, &luigi, &shell);
		}

		if(Player2->IsConnected())
		{
			if(Player2->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{ luigi.handleButtons(UP, true, &mario, &shell); }
			else
			{ luigi.handleButtons(UP, false, &luigi, &shell); }
			if(Player2->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{ luigi.handleButtons(SPIN, true, &mario, &shell); }
			else
			{ luigi.handleButtons(SPIN, false, &mario, &shell); }
			if(Player2->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{ luigi.handleButtons(GRAB, true, &mario, &shell); }
			else
			{ luigi.handleButtons(GRAB, false, &mario, &shell); }
			if(Player2->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{ luigi.handleButtons(LEFT_CK, true, &mario, &shell); }
			else
			{ luigi.handleButtons(LEFT_CK, false, &mario, &shell);}
			if(Player2->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{ luigi.handleButtons(RIGHT_CK, true, &mario, &shell); }
			else
			{ luigi.handleButtons(RIGHT_CK, false, &mario, &shell);}
			if(Player2->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{ PostMessage(hWnd, WM_CLOSE, 0, 0); }
		}
		else
		{
			if (keys[VK_NUMPAD8] & 0x80)
				luigi.handleButtons(UP, true, &mario, &shell);
			else
				luigi.handleButtons(UP, false, &mario, &shell);
			if (keys[VK_NUMPAD5] & 0x80)
				luigi.handleButtons(BLOCK, true, &mario, &shell);
			else
				luigi.handleButtons(BLOCK, false, &mario, &shell);
			if (keys[VK_NUMPAD4] & 0x80)
				luigi.handleButtons(LEFT_CK, true, &mario, &shell);
			else
				luigi.handleButtons(LEFT_CK, false, &mario, &shell);
			if (keys[VK_NUMPAD6] & 0x80)
				luigi.handleButtons(RIGHT_CK, true, &mario, &shell);
			else
				luigi.handleButtons(RIGHT_CK, false, &mario, &shell);
			if (keys['O'] & 0x80)
				luigi.handleButtons(GRAB, true, &mario, &shell);
			else
				luigi.handleButtons(GRAB, false, &mario, &shell);
			if (keys['P'] & 0x80)
				luigi.handleButtons(SPIN, true, &mario, &shell);
			else
				luigi.handleButtons(SPIN, false, &mario, &shell);
		}
		if(keys[VK_SPACE] & 0x80)
		{
			spacePressed = true;
		}
		else
		{
			if(fullScreen && spacePressed)
			{
				ToggleFullscreenMode(FALSE, 1042, 808, 0, 0);
				fullScreen = false;
			}
			else if (!fullScreen && spacePressed)
			{
				fullScreen = true;
				ToggleFullscreenMode(TRUE, 0, 0, 0, 0);
			}
			spacePressed = false;
		}
		break;
	}
}

void Game::Simulate(void)
{
	//TODO: Simulate AI
	if(mario.score >= 5)
	{
		int response = MessageBox(hWnd, _T("Mario Wins the Game!\nPlay Again?"),
			_T("Program Name"), MB_YESNO | MB_ICONQUESTION);
		if (response == IDYES)
		{
			// TODO: Reset the board
			reset();
		}
		else if(response == IDNO)
		{
			// Begin the Windows Shutdown sequence.
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
	if(luigi.score >= 5)
	{
		int response = MessageBox(hWnd, _T("Luigi Wins the Game!\nPlay Again?"),
			_T("Program Name"), MB_YESNO | MB_ICONQUESTION);
		if (response == IDYES)
		{
			// TODO: Reset the board
			reset();
		}
		else if(response == IDNO)
		{
			// Begin the Windows Shutdown sequence.
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}

	//TODO: Simulate Physics
	mario.updatePos(platform, &luigi, &shell);
	luigi.updatePos(platform, &mario, &shell);
	shell.updatePos(platform, &mario, &luigi);

	//TODO: Simulate Collision Detection & Response


	//// Rectangle to Rectangle Intersection:
	//RECT rIntersect;
	//RECT rBall = {(LONG)ball.loc.x, (LONG)ball.loc.y, (LONG)ball.loc.x + (LONG)ball.size.cx, (LONG)ball.loc.y + (LONG)ball.size.cy};
	////RECT rPaddle = {(LONG)paddlepos.left, (LONG)paddlepos.top, (LONG)paddlepos.right, (LONG)paddlepos.bottom};
	//if(IntersectRect(&rIntersect, &rBall, &rPaddle))
	//{
	//	// TODO: Intersection Response.
	//	ballvec.width = -ballvec.width;

	//}
}

void Game::Render(void)
{
	pRT->BeginDraw();

	//TODO: Clear Back Buffer
	pRT->Clear(D2DColor(CornflowerBlue));

	switch(gamestate)
	{
	case MENU:
		drawMenu();
		break;
	case PLAY:
		drawPlay();
		break;
	}

	//TODO: Present Back Buffer

	HRESULT hr = pRT->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		//TODO: Destroy Direct2D.
		DestroyGraphics();
		//TODO: Recreate Direct2D.
		CreateGraphics(hWnd);
	}

	static DWORD spriteTime = GetTickCount();

	if(GetTickCount() - spriteTime > 100)
	{
		// Update frame counter (based on time)
		mario.updateFrame();
		luigi.updateFrame();
		shell.updateFrame();

		spriteTime = GetTickCount();
	}
}

HRESULT Game::CreateGraphics(HWND hWnd)
{
	// Initialize the Direct2D Factory.
	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Factory."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}


	// Get the dimensions of the client.
	RECT rc;
	GetClientRect(hWnd, &rc);

	// Initialize a Direct2D Size Structure.
	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);




	// Create the Direct2D Render Target.
	hr = pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, size), &pRT);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Render Target."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
	pRT->SetDpi(96.0f, 96.0f);


	// Create the Direct2D Solid Color Brush.
	hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0x0), &pBrush);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Solid Color Brush."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}


	// Initialize the DirectWrite Factory.
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		(IUnknown**)&pDWFactory);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Factory."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}


	// Create the DirectWrite Text Format.
	hr = pDWFactory->CreateTextFormat(_T("Veranda"), NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		24, _T(""), &pTF);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Text Format."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	hr = CoCreateInstance(
#if _MSC_VER >= 1700
		CLSID_WICImagingFactory1,
#else
		CLSID_WICImagingFactory,
#endif
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pWICFactory));

	//// Create a bitmap by loading it from a file.
	hr = LoadBitmapFromFile(_T("backgroundMario.jpg"), &backGround_bmp);
	if (FAILED(hr) )
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to load the background."),
			_T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
	else
		backGround.setStats(backGround_bmp, 0, 0, 1600, 1200);

	hr = LoadBitmapFromFile(_T("block_16x16.png"), &blocks_bmp);
	if (FAILED(hr) )
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to load the blocks."),
			_T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
	else
		blocks.setStats(blocks_bmp, 0, 0, 16, 16);

	hr = LoadBitmapFromFile(_T("mario_sheet_255x255.png"), &mario_bmp);
	if (FAILED(hr) )
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to load the blocks."),
			_T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
	else
	{
		mario.setAnime(mario_bmp, 192, 0, 32, 32, 2,
			mario_bmp, 128, 64, 32, 32, 4,
			mario_bmp, 192, 128, 32, 32, 2);
		mario.setStill(mario_bmp, 0, 0, 32, 32,
			mario_bmp, 0, 32, 32, 32,
			mario_bmp, 0, 64, 32, 32,
			mario_bmp, 0, 96, 32, 32,
			mario_bmp, 0, 128, 32, 32,
			mario_bmp, 0, 160, 32, 32);

		luigi.setAnime(mario_bmp, 192, 32, 32, 32, 2,
			mario_bmp, 128, 96, 32, 32, 4,
			mario_bmp, 192, 160, 32, 32, 2);
		luigi.setStill(mario_bmp, 32, 0, 32, 32,
			mario_bmp, 32, 32, 32, 32,
			mario_bmp, 32, 64, 32, 32,
			mario_bmp, 32, 96, 32, 32,
			mario_bmp, 32, 128, 32, 32,
			mario_bmp, 32, 160, 32, 32);

		shell.setAnime(mario_bmp, 96, 0, 16, 16, 4);
		shell.setStill(mario_bmp, 96, 0, 16, 16);

		woodBlock.setStats(mario_bmp, 64, 0, 16, 16);
		jumpBlock.setStats(mario_bmp, 80, 0, 16, 16);
		gPipe0.setStats(mario_bmp, 64, 16, 16, 16);
		gPipe1.setStats(mario_bmp, 80, 16, 16, 16);
		gPipe2.setStats(mario_bmp, 64, 32, 16, 16);
		gPipe3.setStats(mario_bmp, 80, 32, 16, 16);
		rPipe0.setStats(mario_bmp, 64, 48, 16, 16);
		rPipe1.setStats(mario_bmp, 80, 48, 16, 16);
		rPipe2.setStats(mario_bmp, 64, 64, 16, 16);
		rPipe3.setStats(mario_bmp, 80, 64, 16, 16);


	}

	hr = LoadBitmapFromFile(_T("menu.png"), &menu_bmp);
	if (FAILED(hr) )
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to load the menu."),
			_T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}



	//pTF->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	//pTF->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	//pTF->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	return S_OK; // Success!
}

void Game::DestroyGraphics(void)
{
	SafeRelease(&pTF);
	SafeRelease(&pDWFactory);
	SafeRelease(&pBrush);
	SafeRelease(&pRT);
	// Release the Direct2D Factory.
	SafeRelease(&pD2DFactory);
}

HRESULT Game::LoadBitmapFromFile(LPCTSTR strFileName, ID2D1Bitmap ** ppBitmap)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICFormatConverter *pConverter = NULL;

	HRESULT hr = pWICFactory->CreateDecoderFromFilename(
		strFileName,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		);

	if (SUCCEEDED(hr))
	{
		//pDecoder->GetFrameCount();
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{

		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pWICFactory->CreateFormatConverter(&pConverter);

	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRT->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}


	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pConverter);

	return hr;
}

HRESULT Game::LoadBitmapFromResource(LPCTSTR strResourceName,
									 LPCTSTR strResourceType, ID2D1Bitmap** ppBitmap)
{
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICStream *pStream = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void *pImageFile = NULL;
	DWORD imageFileSize = 0;
	HINSTANCE hinst = GetModuleHandle(NULL);


	// Locate the resource.
	imageResHandle = FindResourceW(hinst, strResourceName, strResourceType);
	HRESULT hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(hinst, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}
	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(hinst, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;

	}
	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pWICFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
			);
	}
	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
			);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Create the pixel format converter component.
		hr = pWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
	{

		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRT->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap);
	}

	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pSource);
	SafeRelease(&pDecoder);

	return S_OK;
}


void Game::openMap(string filePath)
{
	ifstream fin;
	fin.open(filePath);
	if(fin.is_open())
	{
		int tileSizeWidth, tileSizeHeight, tileSetSizeWidth, tileSetSizeHeight, mapSizeWidth, mapSizeHeight;
		fin >> tileSizeWidth >> tileSizeHeight >> tileSetSizeWidth >> tileSetSizeHeight >> mapSizeWidth >> mapSizeHeight;
		for(int x = 0; x < mapSizeWidth; x++)
			for(int y = 0; y < mapSizeHeight; y++)
			{
				int picIndex, solid, faceRight, g;
				fin >> picIndex >> solid >> faceRight >> g;
				GOAL goal;
				if(g == 0)
					goal = MARIO;
				else if(g == 1)
					goal = LUIGI;
				else
					goal = NOBODY;

				Still still;
				Static_Object obj;
				switch(picIndex)
				{
				case -1:
					break;
				case 0:
					obj.setObject(&woodBlock, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 1:
					obj.setObject(&jumpBlock, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 2:
					obj.setObject(&gPipe0, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 3:
					obj.setObject(&gPipe1, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 4:
					obj.setObject(&gPipe2, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 5:
					obj.setObject(&gPipe3, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 6:
					obj.setObject(&rPipe0, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 7:
					obj.setObject(&rPipe1, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 8:
					obj.setObject(&rPipe2, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				case 9:
					obj.setObject(&rPipe3, x*16, 64 + y*16, 16, 16, solid, true, goal, faceRight);
					break;
				}
				platform.push_back(obj);
			}
			fin.close();
	}
}

// Function that toggles a window between fullscreen and windowed mode:
void Game::ToggleFullscreenMode(BOOL bFullScreen, int iWidth, int iHeight, int iBpp, int iRefreshRate)
{
	static WINDOWPLACEMENT wp = {0};
	static HMENU hMenu = NULL;

	if(bFullScreen)
	{
		// Remember the window position.
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(this->hWnd, &wp);
		// Remove the window's title bar.
		SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_POPUP);
		// Put the changes to the window into effect.
		SetWindowPos(this->hWnd, 0, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		// Remember the menu, then remove it.
		hMenu = GetMenu(this->hWnd);
		SetMenu(this->hWnd, NULL);
		// Switch to the requested display mode.
		//SetDisplayMode(iWidth, iHeight, iBpp, iRefreshRate);
		// Position the window to cover the entire screen.
		SetWindowPos(this->hWnd, HWND_TOPMOST,
			0, 0,
			GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_SHOWWINDOW);
		// Remove the cursor.
		ShowCursor(FALSE);
	}
	else
	{
		// Restore the window's title bar.
		SetWindowLongPtr(this->hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		// Put the changes to the window into effect.
		SetWindowPos(this->hWnd, 0, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		// Restore the window's menu.
		SetMenu(this->hWnd, hMenu);
		// Restore the display mode.
		//SetDisplayMode(0, 0, 0, 0);
		// Restore the window's original position.
		SetWindowPlacement(this->hWnd, &wp);
		// Restore the cursor.
		ShowCursor(TRUE);
	}
}

void Game::reset()
{
	mario.score = 0;
	luigi.score = 0;
	fullScreen = false;
	POINT p;
	p.x = 80;
	p.y = 128;
	SIZE s;
	s.cx = 32;
	s.cy = 32;
	SIZE vel;
	vel.cx = vel.cy = 0;

	mario.setStats(p, s, LEFT, vel, 8);
	p.x = pRT->GetSize().width - 128;
	luigi.setStats(p, s, LEFT, vel, 8);

	POINT point;
	SIZE size;
	point.x = pRT->GetSize().width / 2 - 8;
	point.y = pRT->GetSize().height - 664;
	size.cx = size.cy = 16;
	shell.setStats(point, size);

}

void Game::drawMenu()
{
	pRT->DrawBitmap(menu_bmp);
}
void Game::drawPlay()
{

	//TODO: Render Game Components

	//draw the background
	if(backGround.surface != nullptr)
	{
		pRT->DrawBitmap(backGround.surface,D2D1::RectF(0, 64, pRT->GetSize().width, pRT->GetSize().height),
			0.5f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(backGround.clip.left, backGround.clip.top, backGround.clip.right, backGround.clip.bottom));
	}

	//draw the scoreboard
	wostringstream stream;
	stream << " Mario | Luigi";
	D2D1_RECT_F rect;
	rect.left = pRT->GetSize().width / 2 - 80;
	rect.top = 0;
	rect.right = pRT->GetSize().width / 2 + 128;
	rect.bottom = pRT->GetSize().height;
	pBrush->SetColor(D2DColor(Black));
	pRT->DrawTextW(stream.str().c_str(), stream.str().length(), pTF, rect, pBrush);

	for(int x = 0; x < mario.score; x++)
	{
		pRT->DrawBitmap(shell.currStill->surface, D2D1::RectF(pRT->GetSize().width/2 - 128 - x*16, 16, pRT->GetSize().width/2 - 112 - x*16, 32),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(shell.currStill->clip.left,
			shell.currStill->clip.top,
			shell.currStill->clip.right,
			shell.currStill->clip.bottom));
	}
	for(int x = 0; x < luigi.score; x++)
	{
		pRT->DrawBitmap(shell.currStill->surface, D2D1::RectF(pRT->GetSize().width/2 + 128 + x*16, 16, pRT->GetSize().width/2 + 144 + x*16, 32),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(shell.currStill->clip.left,
			shell.currStill->clip.top,
			shell.currStill->clip.right,
			shell.currStill->clip.bottom));
	}


	//draw the platforms
	for(unsigned int i = 0; i < platform.size(); i++)
	{
		if(platform[i].img->surface != nullptr)
		{
			if(platform[i].faceRight)
			{
				pRT->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(platform[i].quad.left + 8, platform[i].quad.top + 8)));
			}
			pRT->DrawBitmap(platform[i].img->surface, D2D1::RectF(platform[i].quad.left, platform[i].quad.top, platform[i].quad.right, platform[i].quad.bottom),
				1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				D2D1::RectF(platform[i].img->clip.left, platform[i].img->clip.top,
				platform[i].img->clip.right, platform[i].img->clip.bottom));
			if(platform[i].faceRight)
				pRT->SetTransform(D2D1::IdentityMatrix());
		}
	}

	//draw the shell
	if(shell.ballStance == MOVING)
	{
		pRT->DrawBitmap(shell.currAnime->surface, D2D1::RectF(shell.loc.x, shell.loc.y, shell.loc.x + shell.size.cx, shell.loc.y + shell.size.cy),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(shell.currAnime->clip[shell.currFrame].left,
			shell.currAnime->clip[shell.currFrame].top,
			shell.currAnime->clip[shell.currFrame].right,
			shell.currAnime->clip[shell.currFrame].bottom));
	}
	else if(shell.ballStance == NOTMOVING)
	{
		pRT->DrawBitmap(shell.currStill->surface, D2D1::RectF(shell.loc.x, shell.loc.y, shell.loc.x + shell.size.cx, shell.loc.y + shell.size.cy),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(shell.currStill->clip.left,
			shell.currStill->clip.top,
			shell.currStill->clip.right,
			shell.currStill->clip.bottom));
	}

	//draw mario
	if(mario.face == RIGHT)
		pRT->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F( (mario.loc.x + (mario.size.cx/2) ), (mario.loc.y + (mario.size.cy/2)))));

	if(mario.stance == ANIME)
	{
		pRT->DrawBitmap(mario.currAnimation->surface, D2D1::RectF(mario.loc.x, mario.loc.y, mario.loc.x + mario.size.cx, mario.loc.y + mario.size.cy),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(mario.currAnimation->clip[mario.currFrame].left,
			mario.currAnimation->clip[mario.currFrame].top,
			mario.currAnimation->clip[mario.currFrame].right,
			mario.currAnimation->clip[mario.currFrame].bottom));
	}
	else if(mario.stance == STILL)
	{
		pRT->DrawBitmap(mario.currStill->surface, D2D1::RectF(mario.loc.x, mario.loc.y, mario.loc.x + mario.size.cx, mario.loc.y + mario.size.cy),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(mario.currStill->clip.left,
			mario.currStill->clip.top,
			mario.currStill->clip.right,
			mario.currStill->clip.bottom));
	}

	if(mario.face == RIGHT)
		pRT->SetTransform(D2D1::IdentityMatrix());

	//draw luigi
	if(luigi.face == RIGHT)
		pRT->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F( (luigi.loc.x + (luigi.size.cx/2) ), (luigi.loc.y + (luigi.size.cy/2)))));

	if(luigi.stance == ANIME)
	{
		pRT->DrawBitmap(luigi.currAnimation->surface, D2D1::RectF(luigi.loc.x, luigi.loc.y, luigi.loc.x + luigi.size.cx, luigi.loc.y + luigi.size.cy),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(luigi.currAnimation->clip[luigi.currFrame].left,
			luigi.currAnimation->clip[luigi.currFrame].top,
			luigi.currAnimation->clip[luigi.currFrame].right,
			luigi.currAnimation->clip[luigi.currFrame].bottom));
	}
	else if(luigi.stance == STILL)
	{
		pRT->DrawBitmap(luigi.currStill->surface, D2D1::RectF(luigi.loc.x, luigi.loc.y, luigi.loc.x + luigi.size.cx, luigi.loc.y + luigi.size.cy),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(luigi.currStill->clip.left,
			luigi.currStill->clip.top,
			luigi.currStill->clip.right,
			luigi.currStill->clip.bottom));
	}

	if(luigi.face == RIGHT)
		pRT->SetTransform(D2D1::IdentityMatrix());

	// Render the Right Paddle.

}