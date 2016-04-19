#include "Enemy.h"


Enemy::Enemy()
{
	hit = false;

	this->rect.x = rand() % 700;
	this->rect.y = -200;
	this->rect.w = 100;
	this->rect.h = 100;

	// Enemy ship information
	eFlyAnimWidth = 64;
	eFlyAnimHeight = 32;
	eFlyFrame = 0;
	eFlyLoop = 0;
	// Explosion information
	expAnimWidth = 80;
	expAnimHeight = 16;
	expFrame = 0;
	expLoop = 0;
}

Enemy::~Enemy()
{

}

void Enemy::CutAnimations()
{
	for (int x = 0; x < eFlyAnimWidth; x = x + (eFlyAnimWidth / eFlyAnims)) {
		eFlyRectSrc[eFlyLoop].x = x;
		eFlyRectSrc[eFlyLoop].y = 0;
		eFlyRectSrc[eFlyLoop].w = eFlyAnimWidth / eFlyAnims;
		eFlyRectSrc[eFlyLoop].h = eFlyAnimHeight;
		eFlyLoop++;
	}
}

void Enemy::CutAnimExplode()
{
	for (int x = 0; x < expAnimWidth; x = x + (expAnimWidth / expAnims)) {
		expRectSrc[expLoop].x = x;
		expRectSrc[expLoop].y = 0;
		expRectSrc[expLoop].w = expAnimWidth / expAnims;
		expRectSrc[expLoop].h = expAnimHeight;
		expLoop++;
	}
}

void Enemy::render(SDL_Renderer *ren, SDL_Texture *eTexture)
{
	eFlyClip = &eFlyRectSrc[eFlyFrame / 10];

	SDL_RenderCopy(ren, eTexture, eFlyClip, &rect);

	// update enemy frame
	++eFlyFrame;
	if (eFlyFrame / 10 >= eFlyAnims)
		eFlyFrame = 0;
}

void Enemy::renderExplosion(SDL_Renderer *ren, SDL_Texture *explosion, SDL_Rect bullet)
{
	expClip = &expRectSrc[expFrame / 10];
	//SDL_Rect *tempRect = &rect;
	if (hit)
		SDL_RenderCopy(ren, explosion, expClip, &bullet);

	// update enemy frame and don't reset it cause we don't want to repeat
	++expFrame;
}

void Enemy::Respawn()
{
	this->rect.x = rand() % 700;
	this->rect.y = -200;
	//this->hit = false;
}

void Enemy::moveStep()
{
	this->rect.y++;
}