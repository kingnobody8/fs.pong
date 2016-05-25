#pragma once

#include <d2d1.h> // Include the Direct2D API.
#pragma comment(lib, "d2d1.lib") // Connect to the Direct2D Import Library.

#include <wincodec.h> // Include the Windows Imaging Component (WIC).
#pragma comment(lib, "windowscodecs.lib") // Connect to WIC Import Library.

#include "Static_Object.h"
#include "Still.h"
#include "Animation.h"
#include "Entity.h"
//#include "Audio_SDL.h"

#include <vector>
using namespace std;

enum BALLSTANCE { MOVING, NOTMOVING};
enum BALLSTATE {SITTING, HELD, DROPPING, ROLLING };

struct Entity;

struct Ball
{
	Still sit;
	Animation roll;
	Still * currStill;
	Animation * currAnime;

	Entity * ballHolder;

	POINT loc;
	SIZE size;

	SIZE vel;
	SIZE acc;

	int currFrame;
	BALLSTANCE ballStance;
	BALLSTATE ballState;

	Ball(void);
	~Ball(void);

	void setAnime(ID2D1Bitmap* roll, int r_xLoc, int r_yLoc, int r_width, int r_height, int r_numFrames);
	void setStill(ID2D1Bitmap* sit, int s_xLoc, int s_yLoc, int s_width, int s_height);

	void setStats( POINT loc, SIZE size);
	void updatePos(vector<Static_Object> platform, Entity *mario, Entity *luigi);

	bool platCollideVert(vector<Static_Object> platform);
	bool platCollideHorz(vector<Static_Object> platform);
	bool entityCollideVert(Entity *person);
	bool entityCollideHorz(Entity *person);

	void updateFrame();

	
	GOAL checkGoal(vector<Static_Object> platform);
};