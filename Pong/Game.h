#pragma once

#include <Windows.h> // Include the Win32 API
#include <tchar.h> // Include support for UNICODE
#include <sstream> // Include wostringstream support
#include "CXBOXController.h"
using std::wostringstream;

#include <fstream>

#include <d2d1.h> // Include the Direct2D API.
#pragma comment(lib, "d2d1.lib") // Connect to the Direct2D Import Library.

#include <dwrite.h> // Include the DirectWrite API.
#pragma comment(lib, "dwrite.lib") // Connect to the DirectWrite Import Library.

#include <mmsystem.h> // Include the Multimedia header file.
#pragma comment(lib, "winmm.lib") // Connect to the Multimedia import library.

#include <wincodec.h> // Include the Windows Imaging Component (WIC).
#pragma comment(lib, "windowscodecs.lib") // Connect to WIC Import Library.

#include "resource.h"


#include "Entity.h"
#include "Ball.h"
#include "Still.h"
#include "Animation.h"
#include "Static_Object.h"

enum GAMESTATE {MENU, PLAY};

class Game
{
	CXBOXController* Player1;
	CXBOXController* Player2;
public:

	GAMESTATE gamestate;
private:

	// Window Handle:
	HWND hWnd;

	// DirectX Helper Functions

	// The SafeRelease Pattern
	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}

	// Programmer defined macro to make using the known colors easier.
#define D2DColor(clr) D2D1::ColorF(D2D1::ColorF::clr)

	HRESULT CreateGraphics(HWND hWnd);
	void DestroyGraphics(void);

	// Creates a Direct2D bitmap from a file.
	HRESULT LoadBitmapFromFile(LPCTSTR strFileName, ID2D1Bitmap ** ppBitmap);
	// Creates a Direct2D bitmap from a resource in the application resource file.
	HRESULT LoadBitmapFromResource(LPCTSTR strResourceName, LPCTSTR strResourceType, ID2D1Bitmap** ppBitmap);

	// DirectX Interfaces:
	ID2D1Factory* pD2DFactory;
	ID2D1HwndRenderTarget* pRT;
	ID2D1SolidColorBrush *pBrush;

	D2D1_COLOR_F custom;
	D2D1_POINT_2F ptmouse;

	// DirectWrite Interfaces:
	IDWriteFactory* pDWFactory;
	IDWriteTextFormat* pTF;

	// Windows Imaging Component Interface:
	IWICImagingFactory* pWICFactory;

	// Game Components:
	Still backGround;
	ID2D1Bitmap* backGround_bmp;
	Still blocks;
	ID2D1Bitmap* blocks_bmp;

	ID2D1Bitmap* mario_bmp;
	ID2D1Bitmap* luigi_bmp;
	ID2D1Bitmap* shell_bmp;
	ID2D1Bitmap* menu_bmp;

	Entity mario, luigi;
	Ball shell;

	Still woodBlock, jumpBlock, gPipe0, gPipe1, gPipe2, gPipe3, rPipe0, rPipe1, rPipe2, rPipe3;


	vector<Static_Object> platform;


	// Variables used to calculate frames per second:
	DWORD dwFrames;
	DWORD dwCurrentTime;
	DWORD dwLastUpdateTime;
	DWORD dwElapsedTime;
	std::wstring szFPS;

	bool fullScreen;


	//----------------------------------------------------


public:
	BOOL bRunning;		// Is the game running?
	HANDLE hGameThread; // Handle to the GameMain thread.
	BYTE keys[256];		// Current state of the keyboard.


	Game(void);
	virtual ~Game(void);

	void Startup(void);
	void Shutdown(void);
	void Input(void);
	void Simulate(void);
	void Render(void);

	void SetHwnd(HWND _hWnd) {hWnd = _hWnd;}

	void openMap(string filePath);

	void ToggleFullscreenMode(BOOL bFullScreen, int iWidth, int iHeight, int iBpp, int iRefreshRate);

	bool getMute(void)
	{
		return false; //mario.audio.mute;
	}
	void setMute(bool set)
	{
		//mario.audio.mute = set;
		//luigi.audio.mute = set;
	}

	void reset();

	void drawMenu();
	void drawPlay();
};
