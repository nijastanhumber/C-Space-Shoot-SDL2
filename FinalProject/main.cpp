#include "Game.h"

int main(int argc, char *argv[])
{
	Game *newGame = new Game();
	newGame->init();
	newGame->runGame();
	newGame->destroy();

	delete newGame;

	return 0;
}
