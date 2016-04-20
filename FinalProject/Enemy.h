#ifndef ENEMY_H
#define ENEMY_H

#include "Constants.h"

class Enemy
{
private:
	const int eFlyAnims = 2;
	int eFlyAnimWidth;
	int eFlyAnimHeight;
	int eFlyLoop;
	SDL_Rect eFlyRectSrc[2];
	int eFlyFrame;
	SDL_Rect *eFlyClip;

	const int expAnims = 5;
	int expAnimWidth;
	int expAnimHeight;
	int expLoop;
	SDL_Rect expRectSrc[5];
	
	SDL_Rect *expClip;

public:
	Enemy();
	~Enemy();
	void CutAnimations();
	void CutAnimExplode();
	void render(SDL_Renderer *ren, SDL_Texture *eTexture);
	void renderExplosion(SDL_Renderer *ren, SDL_Texture *explosion, SDL_Rect bullet);

	SDL_Rect rect;
	void Respawn();

	void moveStraight();

	int expFrame;
	bool hit;
};

#endif
