#pragma once

#include <d2d1.h> // Include the Direct2D API.
#pragma comment(lib, "d2d1.lib") // Connect to the Direct2D Import Library.

#include <wincodec.h> // Include the Windows Imaging Component (WIC).
#pragma comment(lib, "windowscodecs.lib") // Connect to WIC Import Library.

#include "Static_Object.h"
#include "Still.h"
#include "Animation.h"
#include "Ball.h"
//#include "Audio_SDL.h"


#include <vector>
using namespace std;

enum FACE { LEFT, RIGHT};
enum STANCE { ANIME, STILL };
enum STATE {GROUNDED, JUMPING, FALLING, HIT, SPINNING };
enum BUTTONS { UP, BLOCK, RIGHT_CK, LEFT_CK, GRAB, SPIN };

struct Ball;

struct Entity
{
	Still stand, jump, fall, block, change, kick, hold;
	Animation spin, walk, hold_walk;
	Still* currStill;
	Animation* currAnimation;

	//Audio_SDL audio;

	bool holding_Shell;
	bool spun;

	POINT loc;
	SIZE size;
	int Xoffset;

	FACE face;
	STANCE stance;
	STATE state;

	SIZE vel;
	SIZE acc;

	int currFrame;
	int score;

	bool upPress, blockPress, leftPress, rightPress, spinPress, grabPress;


public:

	Entity(void);
	~Entity(void);

	void setAnime(ID2D1Bitmap* walk, int w_xLoc, int w_yLoc, int w_width, int w_height, int w_numFrames,
		ID2D1Bitmap* spin, int s_xLoc, int s_yLoc, int s_width, int s_height, int s_numFrames,
		ID2D1Bitmap* hold_walk, int hw_xLoc, int hw_yLoc, int hw_width, int hw_height, int hw_numFrames);
	void setStill(ID2D1Bitmap* stand, int s_xLoc, int s_yLoc, int s_width, int s_height,
		ID2D1Bitmap* jump, int j_xLoc, int j_yLoc, int j_width, int j_height,
		ID2D1Bitmap* fall, int f_xLoc, int f_yLoc, int f_width, int f_height,
		ID2D1Bitmap* block, int b_xLoc, int b_yLoc, int b_width, int b_height,
		ID2D1Bitmap* change, int c_xLoc, int c_yLoc, int c_width, int c_height,
		ID2D1Bitmap* hold, int h_xLoc, int h_yLoc, int h_width, int h_height);

	void setStats( POINT loc, SIZE size, FACE face, SIZE vel, int xOffset);

	void updatePos(vector<Static_Object> platform, Entity *otherPlayer, Ball *ball);
	void updateVelocityControls(FACE direction, Entity *otherPlayer);

	void handleButtons(BUTTONS press, bool held, Entity * otherPlayer, Ball * ball);

	void handleUpKey(BUTTONS button, Ball * ball);

	bool platCollideVert(vector<Static_Object> platform);
	bool platCollideHorz(vector<Static_Object> platform);
	bool entityCollideVert(Entity *person);
	bool entityCollideHorz(Entity *person, FACE direction);
	bool shellCollideVert(Ball * ball);
	bool shellCollideHorz(Ball * ball, Entity * otherPlayer);

	bool spinAttack(Entity * otherPlayer);

	void updateFrame();

};