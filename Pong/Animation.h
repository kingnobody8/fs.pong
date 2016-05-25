#pragma once

#include <d2d1.h> // Include the Direct2D API.
#pragma comment(lib, "d2d1.lib") // Connect to the Direct2D Import Library.

#include <mmsystem.h> // Include the Multimedia header file.
#pragma comment(lib, "winmm.lib") // Connect to the Multimedia import library.

struct Animation
{
	ID2D1Bitmap* surface;
	RECT * clip;
	unsigned int numFrames;

	void setStats(ID2D1Bitmap* surface = nullptr, int xLoc = 0, int yLoc = 0, int width = 0, int height = 0, int numFrames = 0)
	{
		this->surface = surface;
		this->numFrames = numFrames;
		if(numFrames != 0)
		{
			this->clip = new RECT[numFrames];
			for(unsigned int i = 0; i < this->numFrames; i++)
			{
				this->clip[i].left = xLoc + i * width;
				this->clip[i].top = yLoc;
				this->clip[i].right = this->clip[i].left + width;
				this->clip[i].bottom = yLoc + height;
			}
		}
		else
			this->clip = nullptr;
	}

	~Animation(void)
	{
		delete [] clip;
	}
};