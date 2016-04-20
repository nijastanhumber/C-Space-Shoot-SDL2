#include "Bullet.h"

Bullet::Bullet(int x, int y)
{
	this->rect.x = x;
	this->rect.y = y;
	this->rect.w = 16 * 3;
	this->rect.h = 16 * 3;
}

Bullet::~Bullet()
{
	
}

void Bullet::moveStep()
{
	this->rect.y -= constants::BULLETSPEED;
}
