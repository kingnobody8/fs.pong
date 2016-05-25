#pragma once

#include "Still.h"

struct Static_Object
{
	Still* img;
	RECT quad;
	bool jumpThrough;
	bool solid;
	GOAL goal;
	bool faceRight;

	void setObject( Still * still, int xLoc, int yLoc, int width, int height, int jumpThrough, bool solid, GOAL goal, int faceRight)
	{
		img = still;
		quad.left = xLoc;
		quad.top = yLoc;
		quad.right = xLoc + width;
		quad.bottom = yLoc + height;
		if(jumpThrough == 0)
			this->jumpThrough = false;
		else
			this->jumpThrough = true;
		if(faceRight == 0)
			this->faceRight = false;
		else
			this->faceRight = true;
		this->solid = solid;
		this->goal = goal;
	}


};