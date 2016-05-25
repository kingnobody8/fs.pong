#include "Entity.h"

Entity::Entity(void)
{
	loc.x = loc.y = 0;
	size.cx = size.cy = 0;
	vel.cx = vel.cy = 0;
	currFrame = 0;
	face = LEFT;
	state = FALLING;
	stance = STILL;
	currAnimation = &walk;
	currStill = &fall;
	holding_Shell = spun = upPress = blockPress = leftPress = rightPress = grabPress = spinPress = false;


	//audio.startUp();
	// remember to delete
	//audio.mute = true;
}

Entity::~Entity(void)
{
	//audio.cleanUp();
}

void Entity::setAnime(ID2D1Bitmap* walk, int w_xLoc, int w_yLoc, int w_width, int w_height, int w_numFrames,
					  ID2D1Bitmap* spin, int s_xLoc, int s_yLoc, int s_width, int s_height, int s_numFrames,
					  ID2D1Bitmap* hold_walk, int hw_xLoc, int hw_yLoc, int hw_width, int hw_height, int hw_numFrames)
{
	this->walk.setStats(walk, w_xLoc, w_yLoc, w_width, w_height, w_numFrames);
	this->spin.setStats(spin, s_xLoc, s_yLoc, s_width, s_height, s_numFrames);
	this->hold_walk.setStats(hold_walk, hw_xLoc, hw_yLoc, hw_width, hw_height, hw_numFrames);
}

void Entity::setStill(ID2D1Bitmap* stand, int s_xLoc, int s_yLoc, int s_width, int s_height,
					  ID2D1Bitmap* jump, int j_xLoc, int j_yLoc, int j_width, int j_height,
					  ID2D1Bitmap* fall, int f_xLoc, int f_yLoc, int f_width, int f_height,
					  ID2D1Bitmap* block, int b_xLoc, int b_yLoc, int b_width, int b_height,
					  ID2D1Bitmap* change, int c_xLoc, int c_yLoc, int c_width, int c_height,
					  ID2D1Bitmap* hold, int h_xLoc, int h_yLoc, int h_width, int h_height)
{
	this->stand.setStats(stand, s_xLoc, s_yLoc, s_width, s_height);
	this->jump.setStats(jump, j_xLoc, j_yLoc, j_width, j_height);
	this->fall.setStats(fall, f_xLoc, f_yLoc, f_width, f_height);
	this->block.setStats(block, b_xLoc, b_yLoc, b_width, b_height);
	this->change.setStats(change, c_xLoc, c_yLoc, c_width, c_height);
	this->hold.setStats(hold, h_xLoc, h_yLoc, h_width, h_height);
}

void Entity::setStats( POINT loc, SIZE size, FACE face, SIZE vel, int xOffset)
{
	this->loc = loc;
	this->size = size;
	this->face = face;
	this->vel = vel;
	this->acc.cy = 1;
	this->currFrame = 0;
	this->Xoffset = xOffset;
}


void Entity::updatePos(vector<Static_Object> platform, Entity *otherPlayer, Ball *ball)
{
	if(loc.x + size.cx < 0 || loc.y + size.cy < 0 || loc.y > 2000)
	{
		loc.x = 464;
		loc.y = 64;
	}
	vel.cx += acc.cx;
	loc.x += vel.cx;
	if(platCollideHorz(platform))
	{ vel.cx = 0; }
	switch(state)
	{
	case JUMPING:
		{
			if(holding_Shell)
				currStill = &hold;
			else
				currStill = &jump;

			vel.cy += acc.cy;
			if(vel.cy > 4)
				vel.cy = 4;
			loc.y += vel.cy;
			if(platCollideVert(platform) || vel.cy >= 0)
			{
				if(holding_Shell)
					currStill = &hold;
				else
					currStill = &fall;
				state = FALLING;
				vel.cy = 0;
			}
			stance = STILL;
			break;
		}
	case FALLING:
		{
			vel.cy += acc.cy;
			loc.y += vel.cy;
			entityCollideVert(otherPlayer);
			shellCollideVert(ball);
			if(platCollideVert(platform))
			{
				if(holding_Shell)
					currStill = &hold;
				else
					currStill = &stand;
				state = GROUNDED;
				vel.cy = 0;
			}
			stance = STILL;
			break;
		}
	case GROUNDED:
		{
			if(state == STILL)
			{
				if(holding_Shell)
					currStill = &hold;
				else
					currStill = &stand;
			}
			else if(state == ANIME)
			{
				if(holding_Shell)
					currAnimation = &hold_walk;
				else
					currAnimation = &walk;
			}
			if(!platCollideVert(platform))
			{
				stance = STILL;
				if(holding_Shell)
					currStill = &hold;
				else
					currStill = &fall;
				state = FALLING;
			}

			break;
		}
	case SPINNING:
		{
			if(spun)
			{
				stance = STILL;
				currStill = &fall;
				state = FALLING;
				spun = false;
			}
			else
			{
				stance = ANIME;
				currAnimation = &spin;

			}
			if(!platCollideVert(platform))
			{
				vel.cy += acc.cy;
				loc.y += vel.cy;
			}
			entityCollideVert(otherPlayer);
			entityCollideHorz(otherPlayer, this->face);
			shellCollideVert(ball);
			spinAttack(otherPlayer);
			break;
		}
	case HIT:
		{
			if(!platCollideVert(platform))
			{
				vel.cy += acc.cy;
				loc.y += vel.cy;
			}
			entityCollideVert(otherPlayer);
			entityCollideHorz(otherPlayer, this->face);
			shellCollideVert(ball);
			if(currFrame >= 20)
			{
				state = FALLING;
				currStill = &fall;
				stance = STILL;
			}
			break;
		}
	}
}

void Entity::updateVelocityControls(FACE direction, Entity *otherPlayer)
{

	if(state != HIT)
	{
		if(direction == RIGHT)
		{
			if(holding_Shell)
				loc.x += 3;
			else
				loc.x += 4;

			entityCollideHorz(otherPlayer, RIGHT);
			entityCollideVert(otherPlayer);
		}
		else
		{
			if(holding_Shell)
				loc.x -= 3;
			else
				loc.x -= 4;
			entityCollideHorz(otherPlayer, LEFT);
			entityCollideVert(otherPlayer);

		}
	}
}


void Entity::handleButtons(BUTTONS press, bool held, Entity * otherPlayer, Ball * ball)
{
	if(state != HIT)
	{
		switch(press)
		{
		case UP:
			{ 
				if(held)
				{
					upPress = true;
					if(state != JUMPING && state != HIT && state != FALLING && state != SPINNING)
					{
						//audio.playJump();
						//MessageBeep(MB_ICONERROR);
						stance = STILL;
						currStill = &jump;
						state = JUMPING;
						vel.cy = -13;
					}
				}
				else
					handleUpKey(UP, ball);
				break;
			}
		case BLOCK:
			{ 
				if(held)
					blockPress = true;
				else 
					handleUpKey(BLOCK, ball);
				break;
			}
		case RIGHT_CK:
			{
				if(held)
				{
					rightPress = true;
					if(state == GROUNDED)
					{
						if(face == LEFT)
						{ currStill = &change; stance = STILL; }
						else
						{ stance = ANIME; currAnimation = &walk; updateVelocityControls(RIGHT, otherPlayer);}
						face = RIGHT;
					}
					else if(state == JUMPING || state == FALLING || state == SPINNING)
					{
						face = RIGHT;
						updateVelocityControls(RIGHT, otherPlayer);
					}
				}
				else
					handleUpKey(RIGHT_CK, ball);
				break;
			}
		case LEFT_CK:
			{
				if(held)
				{
					leftPress = true;
					if(state == GROUNDED)
					{
						if(face == RIGHT)
						{ currStill = &change; stance = STILL; }
						else
						{ stance = ANIME; currAnimation = &walk; updateVelocityControls(LEFT, otherPlayer);}
						face = LEFT;
					}
					else if(state == JUMPING || state == FALLING || state == SPINNING)
					{
						face = LEFT;
						updateVelocityControls(LEFT, otherPlayer);
					}
				}
				else
					handleUpKey(LEFT_CK, ball);
				break;
			}
		case GRAB:
			{
				if(held)
				{
					grabPress = true;
					if(ball->loc.y + ball->size.cy >= this->loc.y && ball->loc.y <= this->loc.y + this->size.cy && holding_Shell == false && !otherPlayer->holding_Shell)
					{
						if((ball->loc.x + ball->size.cx + 8 >= this->loc.x && ball->loc.x < this->loc.x) ||
							(ball->loc.x - 8 <= this->loc.x + this->size.cx && ball->loc.x > this->loc.x))
						{
							this->holding_Shell = true;
							this->currStill = &hold;
							this->currAnimation = &hold_walk;
							this->stance = STILL;
							ball->ballState = HELD;
						}
					}


				}
				else
				{
					grabPress = false;
					handleUpKey(GRAB, ball);
				}
				break;
			}
		case SPIN:
			{
				if(held)
				{
					//audio.playSpin();
					spinPress = true;
					stance = ANIME;
					currAnimation =  &spin;
					state = SPINNING;
				}
				else
				{
					spinPress = false;
					handleUpKey(SPIN, ball);
				}
				break;
			}
		}
	}
}

void Entity::handleUpKey(BUTTONS button, Ball * ball)
{
	switch(button)
	{
	case UP:
	case BLOCK:
	case LEFT_CK:
		{
			if(leftPress)
			{
				if(state == GROUNDED)
				{
					face = LEFT;
					stance = STILL;
					leftPress = false;
					currStill = &stand;
				}
			}
			break;
		}
	case RIGHT_CK:
		{
			if(rightPress)
			{
				if(state == GROUNDED)
				{
					face = RIGHT;
					stance = STILL;
					rightPress = false;
					currStill = &stand;
				}
			}
			break;
		}
	case GRAB:
		{
			if(holding_Shell)
			{
				//audio.playKick();
				this->holding_Shell = false;
				this->currStill = &stand;
				this->currAnimation = &walk;
				this->stance = STILL;
				ball->ballState = DROPPING;
				if(this->face == LEFT)
					ball->vel.cx = -2;
				else 
					ball->vel.cx = 2;
			}
			break;
		}
	case SPIN:
		break;
	}
}


bool Entity::platCollideVert(vector<Static_Object> platform)
{
	for(int i = 0; i < platform.size(); i++)
	{
		if(this->loc.x + this->Xoffset < platform[i].quad.right && this->loc.x + this->size.cx - this->Xoffset > platform[i].quad.left && platform[i].solid)
		{
			if( this->loc.y + this->size.cy + 1 > platform[i].quad.top && this->loc.y < platform[i].quad.top)
			{
				this->loc.y = platform[i].quad.top - this->size.cy;
				return true;
			}
			if( this->loc.y < platform[i].quad.bottom && this->loc.y + this->size.cy > platform[i].quad.bottom && platform[i].jumpThrough)
			{
				//audio.playBump();
				this->loc.y = platform[i].quad.bottom + 2;
				return true;
			}
		}
	}
	return false;
}

bool Entity::platCollideHorz(vector<Static_Object> platform)
{
	for(int i = 0; i < platform.size(); i++)
	{
		if(this->loc.y < platform[i].quad.bottom && this->loc.y + this->size.cy > platform[i].quad.top && platform[i].solid && platform[i].jumpThrough)
		{
			if(this->loc.x + this->Xoffset < platform[i].quad.right && this->loc.x + this->size.cx - this->Xoffset > platform[i].quad.right)
			{
				this->loc.x = platform[i].quad.right - this->Xoffset;
				return true;
			}
			else if(this->loc.x + this->size.cx - this->Xoffset > platform[i].quad.left && this->loc.x + this->Xoffset < platform[i].quad.left)
			{
				this->loc.x = platform[i].quad.left - this->size.cx + this->Xoffset;
				return true;
			}
		}
	}
	return false;
}

bool Entity::entityCollideVert(Entity *person)
{
	if(this->loc.x + this->Xoffset < person->loc.x + person->size.cx - person->Xoffset && this->loc.x + this->size.cx - this->Xoffset > person->loc.x + person->Xoffset)
	{
		if(this->loc.y + this->size.cy > person->loc.y && this->loc.y < person->loc.y)
		{
			this->loc.y = person->loc.y - this->size.cy - 1;
			if(this->loc.x < person->loc.x)
				this->loc.x = person->loc.x - this->size.cx;
			else
				this->loc.x = person->loc.x + person->size.cx;
			return true;
		}
	}
	return false;
}

bool Entity::entityCollideHorz(Entity *person, FACE direction)
{
	if(this->loc.y < person->loc.y + person->size.cy && this->loc.y + this->size.cy > person->loc.y)
	{
		if(this->loc.x + this->Xoffset < person->loc.x + person->size.cx - person->Xoffset && this->loc.x + this->size.cx - this->Xoffset > person->loc.x + person->size.cx - person->Xoffset)
		{
			this->loc.x = person->loc.x + person->size.cx - person->Xoffset - 5 ;
			return true;
		}
		else if(this->loc.x + this->size.cx - this->Xoffset > person->loc.x + person->Xoffset && this->loc.x + this->Xoffset < person->loc.x + person->Xoffset)
		{
			this->loc.x = person->loc.x - this->size.cx + person->Xoffset  + 5;
			return true;
		}
	}
	return false;
}

bool Entity::shellCollideVert(Ball * ball)
{
	if(this->loc.x + this->Xoffset + 1 > ball->loc.x && this->loc.x + this->size.cx - this->Xoffset - 1 < ball->loc.x + ball->size.cx)
	{
		if(this->loc.y + this->size.cy > ball->loc.y && this->loc.y < ball->loc.y + ball->size.cy)
		{
			this->loc.y = ball->loc.y - this->size.cx;
			if(this->loc.x < ball->loc.x)
				this->loc.x = ball->loc.x - this->size.cx;
			else
				this->loc.x = ball->loc.x + ball->size.cx;
			return true;
		}
	}
	return false;
}

bool Entity::shellCollideHorz(Ball * ball, Entity * otherPlayer)
{
	return false;
}


bool Entity::spinAttack(Entity * otherPlayer)
{
	if(this->loc.y < otherPlayer->loc.y + otherPlayer->size.cy && this->loc.y + this->size.cy > otherPlayer->loc.y && otherPlayer->state != HIT && otherPlayer->holding_Shell)
	{
		if(this->loc.x + this->Xoffset - 6 < otherPlayer->loc.x + otherPlayer->size.cx - otherPlayer->Xoffset && this->loc.x + this->size.cx - this->Xoffset > otherPlayer->loc.x + otherPlayer->size.cx - otherPlayer->Xoffset)
		{
			otherPlayer->holding_Shell = false;
			otherPlayer->currStill = &otherPlayer->block;
			otherPlayer->state = HIT;
			otherPlayer->currFrame = 0;
			otherPlayer->currAnimation = nullptr;
			return true;
		}
		else if(this->loc.x + this->size.cx - this->Xoffset + 6 > otherPlayer->loc.x + otherPlayer->Xoffset && this->loc.x + this->Xoffset < otherPlayer->loc.x + otherPlayer->Xoffset)
		{
			otherPlayer->holding_Shell = false;
			otherPlayer->currStill = &otherPlayer->block;
			otherPlayer->state = HIT;
			otherPlayer->currFrame = 0;
			otherPlayer->currAnimation = nullptr;
			return true;
		}
	}
	return false;
}


void Entity::updateFrame()
{
	currFrame++;
	if(currAnimation != nullptr)
	{
		if(currAnimation == &spin && currFrame >= 4)
			spun = true;
		if(currFrame >= currAnimation->numFrames )
			currFrame = 0;
	}

}
