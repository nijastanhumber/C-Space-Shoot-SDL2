#ifndef PLAYER_H
#define PLAYER_H

#include "Constants.h"

class Player
{
private:
	const int pFlyAnims = 4;
	int pFlyAnimWidth;
	int pFlyAnimHeight;
	int flyLoop;
	int pFlyFrame;

	SDL_Rect pFlyStraightRectSrc[4];
	SDL_Rect pFlyRightRectSrc[4];
	SDL_Rect pFlyLeftRectSrc[4];

	SDL_Rect *pFlyClip;

public:
	Player();
	~Player();
	SDL_Rect rect;
	void CutAnimations();
	void SetAnimation();
	void render(SDL_Renderer *ren, SDL_Texture *pTexture);
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	bool playerStraight;
	bool playerRight;
	bool playerLeft;
	bool playerShoot;
};

#endif
