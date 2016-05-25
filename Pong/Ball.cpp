#include "Ball.h"

Ball::Ball(void)
{
	currStill = &sit;
	currAnime = &roll;
	loc.x = loc.y = 0;
	size.cx = size.cy = 0;
	vel.cx = vel.cy = 0;
	acc.cx = acc.cy = 0;
	currFrame = 0;
	ballStance = NOTMOVING;
	ballState = DROPPING;
	ballHolder = nullptr;
}

Ball::~Ball(void)
{
	//roll.~Animation();
}

void Ball::setAnime(ID2D1Bitmap* roll, int r_xLoc, int r_yLoc, int r_width, int r_height, int r_numFrames)
{
	this->roll.setStats(roll, r_xLoc, r_yLoc, r_width, r_height, r_numFrames);
}

void Ball::setStill(ID2D1Bitmap* sit, int s_xLoc, int s_yLoc, int s_width, int s_height)
{
	this->sit.setStats(sit, s_xLoc, s_yLoc, s_width, s_height);
}

void Ball::setStats( POINT loc, SIZE size)
{
	this->loc = loc;
	this->size = size;
	this->vel.cx = this->vel.cy = 0;
	this->acc.cy = 1;
	this->currFrame = 0;
	this->currAnime = &roll;
	this->currStill = &sit;
	this->ballState = DROPPING;
}

void Ball::updatePos(vector<Static_Object> platform, Entity *mario, Entity *luigi)
{
	if(checkGoal(platform) == MARIO)
	{
		mario->score++;
		mario->holding_Shell = luigi->holding_Shell = false;
		loc.x = 496;
		loc.y = 70;
		this->ballState = DROPPING;
		this->vel.cx = -this->vel.cx;
	}
	else if(checkGoal(platform) == LUIGI)
	{
		luigi->score++;
		mario->holding_Shell = luigi->holding_Shell = false;
		loc.x = 496;
		loc.y = 70;
		this->ballState = DROPPING;
		this->vel.cx = -this->vel.cx;
	}
	if(loc.x + size.cx < 0 || loc.y + size.cy < 0 || loc.y > 2000)
	{
		loc.x = 496;
		loc.y = 70;
	}
	switch(ballState)
	{
	case SITTING:
		{
			if(vel.cx != 0)
			{
				ballState = ROLLING;
				ballStance = MOVING;
			}
			if(entityCollideHorz(mario))
			{
				ballState = ROLLING;
				ballStance = MOVING;
				if(mario->loc.x + mario->Xoffset < this->loc.x)
					vel.cx = 2;
				else
					vel.cx = -2;
			}
			if(entityCollideHorz(luigi))
			{
				ballState = ROLLING;
				ballStance = MOVING;
				if(luigi->loc.x + luigi->Xoffset < this->loc.x)
					vel.cx = 2;
				else
					vel.cx = -2;
			}
			if(vel.cx == 0)
			{
				this->ballStance = NOTMOVING;
			}
			break;
		}
	case DROPPING:
		{
			vel.cy = 5;
			loc.y += vel.cy;
			loc.x += vel.cx;
			if(this->vel.cx == 0)
			{
				this->ballStance = NOTMOVING;
				if(platCollideVert(platform))
					ballState = SITTING;
			}
			else
			{
				this->ballStance = MOVING;
				if(platCollideVert(platform))
					ballState = ROLLING;
				platCollideHorz(platform);
				entityCollideHorz(mario);
				entityCollideHorz(luigi);
				entityCollideVert(mario);
				entityCollideVert(luigi);
			}
			break;
		}
	case HELD:
		{
			if(mario->holding_Shell)
				ballHolder = mario;
			else if(luigi->holding_Shell)
				ballHolder = luigi;
			else
				ballHolder = nullptr;
			if(ballHolder != nullptr)
			{
				this->ballStance = NOTMOVING;
				if(ballHolder->face == LEFT)
				{
					this->loc.x = ballHolder->loc.x + ballHolder->Xoffset - 8;
					this->loc.y = ballHolder->loc.y + 12;
				}
				else if(ballHolder->face == RIGHT)
				{
					this->loc.x = ballHolder->loc.x + ballHolder->size.cx - ballHolder->Xoffset - 6;
					this->loc.y = ballHolder->loc.y + 12;
				}
			}
			break;
		}
	case ROLLING:
		{
			loc.x += vel.cx;

			platCollideHorz(platform);
			if(!platCollideVert(platform))
				this->ballState = DROPPING;
			entityCollideHorz(mario);
			entityCollideHorz(luigi);
		}
	}
}

bool Ball::platCollideVert(vector<Static_Object> platform)
{
	for(int i = 0; i < platform.size(); i++)
	{
		if(this->loc.x < platform[i].quad.right && this->loc.x + this->size.cx > platform[i].quad.left)
		{
			if( this->loc.y + this->size.cy > platform[i].quad.top && this->loc.y < platform[i].quad.top)
			{
				this->loc.y = platform[i].quad.top - this->size.cy;
				return true;
			} 
			/*else if( this->loc.y - 1 < platform[i].quad.bottom && this->loc.y + this->size.cy > platform[i].quad.bottom )
			{
			this->loc.y = platform[i].quad.bottom;
			return true;
			}*/
		}
	}
	return false;
}

bool Ball::platCollideHorz(vector<Static_Object> platform)
{
	for(int i = 0; i < platform.size(); i++)
	{
		if(this->loc.y < platform[i].quad.bottom && this->loc.y + this->size.cy > platform[i].quad.top && platform[i].goal == NOBODY)
		{
			if(this->loc.x < platform[i].quad.right && this->loc.x + this->size.cx > platform[i].quad.right)
			{
				this->loc.x = platform[i].quad.right;
				this->vel.cx = -this->vel.cx;
				return true;
			}
			else if(this->loc.x + this->size.cx > platform[i].quad.left && this->loc.x < platform[i].quad.left)
			{
				this->loc.x = platform[i].quad.left - this->size.cx;
				this->vel.cx = -this->vel.cx;
				return true;
			}
		}
	}
	return false;
}

bool Ball::entityCollideVert(Entity *person)
{
	if(this->loc.x < person->loc.x + person->size.cx - person->Xoffset && this->loc.x + this->size.cx > person->loc.x + person->Xoffset)
	{
		if(this->loc.y + this->size.cy + 1 > person->loc.y && this->loc.y < person->loc.y)
		{
			this->loc.y = person->loc.y - this->size.cy;
			return true;
		}
		else if(this->loc.y - 1 < person->loc.y + person->size.cy && this->loc.y + this->size.cy > person->loc.y + person->size.cy)
		{
			this->loc.y = person->loc.y + person->size.cy;
			return true;
		}
	}
	return false;
}

bool Ball::entityCollideHorz(Entity *person)
{
	if(this->loc.y < person->loc.y + person->size.cy && this->loc.y + this->size.cy > person->loc.y)
	{
		if(this->loc.x < person->loc.x + person->size.cx - person->Xoffset && this->loc.x + this->size.cx > person->loc.x + person->size.cx - person->Xoffset)
		{
			this->loc.x = person->loc.x + person->size.cx - person->Xoffset;
			this->vel.cx = -this->vel.cx;
			return true;
		}
		else if(this->loc.x + this->size.cx > person->loc.x + person->Xoffset && this->loc.x < person->loc.x + person->Xoffset)
		{
			this->loc.x = person->loc.x + person->Xoffset - this->size.cx;
			this->vel.cx = -this->vel.cx;
			return true;
		}
	}
	return false;
}

void Ball::updateFrame()
{
	currFrame++;
	if(currFrame >= currAnime->numFrames )
		currFrame = 0;			
}


GOAL Ball::checkGoal(vector<Static_Object> platform)
{
	for(int i = 0; i < platform.size(); i++)
	{
		if(this->loc.y < platform[i].quad.bottom && this->loc.y + this->size.cy > platform[i].quad.top)
		{
			if(this->loc.x < platform[i].quad.right && this->loc.x + this->size.cx > platform[i].quad.right && platform[i].goal != NOBODY)
			{
				return platform[i].goal;
			}
			else if(this->loc.x + this->size.cx > platform[i].quad.left && this->loc.x < platform[i].quad.left && platform[i].goal != NOBODY)
			{
				return platform[i].goal;
			}
		}
	}
	return NOBODY;
}
