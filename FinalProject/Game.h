#ifndef GAME_H
#define GAME_H

#include "Constants.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include "Timer.h"

class Game
{
private:
	bool gameIsRunning;
	bool paused;
	bool stayInMenu;

	const char *maxEnemies; // Will be our value from XML. Set at 4. Don't change the XML value to be anymore than 6 or game will be too hard
	int numEnemies;

	Player *player;
	Bullet *bullet;
	Enemy *enemies;

	int m; // Menu
	int p; // Pause

	int points = 0;
	int lives = 3;
	float scrollingOffset = 0; // For scrolling background

	// Video related
	SDL_Window *win = NULL; // remember to change
	SDL_Renderer *ren = NULL;

	SDL_Event events;

	// Textures
	SDL_Texture *enemyTexture;
	SDL_Texture *enemyExplosion;
	SDL_Texture *bulletTexture;
	SDL_Texture *pTexture;
	SDL_Texture *background;
	SDL_Texture *gameover;
	SDL_Texture *UIScore;
	SDL_Texture *UILives;
	SDL_Texture *UIHighScore;

	// Frame rate stuff
	Timer fpsTimer;
	Timer capTimer;
	int countedFrames = 0;
	float avgFPS;
	int frameTicks;

	//SDL rects
	SDL_Rect explosionRect;
	SDL_Rect gameoverRect;
	SDL_Rect renderQuad;
	SDL_Rect renderQuad1;
	SDL_Rect UIScoreRect;
	SDL_Rect UILivesRect;
	SDL_Rect UIHighScoreRect;

	// Music and sounds
	Mix_Music *bMusic;
	Mix_Chunk *sExplode;
	Mix_Chunk *sShoot;
	Mix_Chunk *sSelect;

	// Fonts
	TTF_Font *font;

	// UI Text and Saving
	int highscore;
	tinyxml2::XMLElement *pHighScore;
	tinyxml2::XMLDocument doc;
	std::stringstream ssHighScore;
	std::string highScore;
	std::stringstream ssScore;
	std::string score;
	std::stringstream ssLives;
	std::string life;

public:
	Game();
	void init();
	static SDL_Texture *loadTexture(SDL_Renderer *ren, char *name);
	static SDL_Texture *loadText(SDL_Renderer *ren, const char *name, int fontsize);
	bool controls(SDL_Renderer *ren, bool stayInMenu);
	int mainMenu(SDL_Renderer *ren, TTF_Font *font);
	int pauseGame(SDL_Renderer *ren, TTF_Font *font);
	void handlePlayerInputs();
	void runGame();
	void updateGame();
	void renderScene();
	void destroy();

	void capFrameRate();

};

#endif
