#pragma once

#include <d2d1.h> // Include the Direct2D API.
#pragma comment(lib, "d2d1.lib") // Connect to the Direct2D Import Library.

#include <mmsystem.h> // Include the Multimedia header file.
#pragma comment(lib, "winmm.lib") // Connect to the Multimedia import library.

enum GOAL {MARIO, LUIGI, NOBODY};


struct Still
{
	ID2D1Bitmap* surface;
	RECT clip;

	void setStats(ID2D1Bitmap* surface, int xLoc, int yLoc, int width, int height)
	{
		this->surface = surface;
		this->clip.left = xLoc;
		this->clip.top = yLoc;
		this->clip.right = xLoc + width;
		this->clip.bottom = yLoc + height;
	}

};