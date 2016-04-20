#include "Player.h"

Player::Player()
{
	pFlyAnimWidth = 64;
	pFlyAnimHeight = 24;
	flyLoop = 0;
	pFlyFrame = 0;

	this->rect.x = constants::SCREENWIDTH / 2 - ((64 / 4) * 5) / 2;
	this->rect.y = constants::SCREENHEIGHT - (24 * 5);
	this->rect.w = (64 / 4) * 5;
	this->rect.h = 24 * 5;

	playerStraight = true;
	playerRight = false;
	playerLeft = false;
	playerShoot = false;
}

Player::~Player()
{

}

void Player::CutAnimations()
{
	// Fly straight
	for (int x = 0; x < pFlyAnimWidth; x = x + (pFlyAnimWidth / pFlyAnims)) {
		pFlyStraightRectSrc[flyLoop].x = x;
		pFlyStraightRectSrc[flyLoop].y = 0;
		pFlyStraightRectSrc[flyLoop].w = pFlyAnimWidth / pFlyAnims;
		pFlyStraightRectSrc[flyLoop].h = pFlyAnimHeight;
		flyLoop++;
	}

	// Fly right
	flyLoop = 0;
	for (int x = 0; x < pFlyAnimWidth; x = x + (pFlyAnimWidth / pFlyAnims)) {
		pFlyRightRectSrc[flyLoop].x = x;
		pFlyRightRectSrc[flyLoop].y = pFlyAnimHeight;
		pFlyRightRectSrc[flyLoop].w = pFlyAnimWidth / pFlyAnims;
		pFlyRightRectSrc[flyLoop].h = pFlyAnimHeight;
		flyLoop++;
	}

	// Fly left
	flyLoop = 0;
	for (int x = 0; x < pFlyAnimWidth; x = x + (pFlyAnimWidth / pFlyAnims)) {
		pFlyLeftRectSrc[flyLoop].x = x;
		pFlyLeftRectSrc[flyLoop].y = pFlyAnimHeight * 2;
		pFlyLeftRectSrc[flyLoop].w = pFlyAnimWidth / pFlyAnims;
		pFlyLeftRectSrc[flyLoop].h = pFlyAnimHeight;
		flyLoop++;
	}
}

void Player::SetAnimation()
{
	// Set animation clips
	if (playerStraight)
	{
		pFlyClip = &pFlyStraightRectSrc[pFlyFrame / 5];
	}
	else if (playerRight)
	{
		pFlyClip = &pFlyRightRectSrc[pFlyFrame / 5];
	}
	else if (playerLeft)
	{
		pFlyClip = &pFlyLeftRectSrc[pFlyFrame / 5];
	}
}

void Player::render(SDL_Renderer *ren, SDL_Texture *pTexture)
{
	SDL_RenderCopy(ren, pTexture, pFlyClip, &rect);

	// update player frame
	++pFlyFrame;
	if (pFlyFrame / 5 >= pFlyAnims)
		pFlyFrame = 0;
}

void Player::moveLeft()
{
	int speed;
	if (rect.x < 0)
		speed = 0;
	else
		speed = constants::PLAYERSPEED;
	this->rect.x -= speed;
}

void Player::moveRight()
{
	int speed;
	if (rect.x + rect.w > constants::SCREENWIDTH)
		speed = 0;
	else
		speed = constants::PLAYERSPEED;
	this->rect.x += speed;
}

void Player::moveUp()
{
	int speed;
	if (rect.y < 0)
		speed = 0;
	else
		speed = constants::PLAYERSPEED;
	this->rect.y -= speed;
}

void Player::moveDown()
{
	int speed;
	if (rect.y + rect.h > constants::SCREENHEIGHT)
		speed = 0;
	else
		speed = constants::PLAYERSPEED;
	this->rect.y += speed;
}
