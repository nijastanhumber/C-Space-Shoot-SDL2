#ifndef BULLET_H
#define BULLET_H

#include "Constants.h"

class Bullet
{
public:
	Bullet(int x, int y);
	~Bullet();
	void moveStep();

	SDL_Rect rect;
};

#endif
