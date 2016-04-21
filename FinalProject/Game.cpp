#include "Game.h"

// Initialize SDL stuff
Game::Game()
{
	gameIsRunning = true;
	paused = false;

	//countedFrames = 0;

	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
}

void Game::init()
{
	doc.LoadFile("gameData.xml");
	maxEnemies = doc.FirstChildElement("gamedata")->FirstChildElement("maxenemy")->GetText();

	// Load highscore data and put into string to use for UI
	pHighScore = doc.FirstChildElement("gamedata")->FirstChildElement("highscore");
	highscore = atoi(pHighScore->GetText());
	ssHighScore.str(std::string()); // clear stringstream of old variables
	ssHighScore << "HighScore: " << highscore; // input current score in stringstream
	highScore = ssHighScore.str();

	// create window and renderer
	win = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, constants::SCREENWIDTH, constants::SCREENHEIGHT, SDL_WINDOW_SHOWN);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	//SDL_CreateWindowAndRenderer(constants::SCREENWIDTH, constants::SCREENHEIGHT, NULL, &win, &ren);

	// Background music and sound effects
	bMusic = Mix_LoadMUS("backgroundmusic.mp3");
	Mix_PlayMusic(bMusic, -1);
	sExplode = Mix_LoadWAV("Death.wav");
	sShoot = Mix_LoadWAV("PlayerShoot.wav");
	sSelect = Mix_LoadWAV("Select.wav");

	// Set background and gameover rect
	background = loadTexture(ren, "desert-backgorund.png");
	gameover = loadTexture(ren, "gameover.png");
	gameoverRect = { 0, 0, constants::SCREENWIDTH, constants::SCREENHEIGHT };

	// Set font for menu and UI text and highscore
	font = TTF_OpenFont("Roboto-Black.ttf", 18);
	UIScore = loadText(ren, "Score: 0", 16);
	UILives = loadText(ren, "Lives: 3", 16);
	UIHighScore = loadText(ren, highScore.c_str(), 16); // where we add highscore from xml to ui
	UIScoreRect = { 650, 0, 100, 100 };
	UILivesRect = { 0, 0, 100, 100 };
	UIHighScoreRect = { 550, 100, 200, 100 };

	// Initialize player bullet
	bullet = nullptr;
	bulletTexture = loadTexture(ren, "playerbullet.png");
	explosionRect = { 0, 0, 100, 100 }; // where explosion will happen. we just initialize it here but change it after. not real explosion parameters

	// Initialize enemies
	numEnemies = atoi(maxEnemies); // Get number of enemies from xml file
	enemies = new Enemy[numEnemies];
	enemyTexture = loadTexture(ren, "enemy-big.png");
	enemyExplosion = loadTexture(ren, "explosion.png");

	// Cut out enemy fly and exploding animations
	for (int i = 0; i < numEnemies; i++) {
		enemies[i].CutAnimations();
		enemies[i].CutAnimExplode();
	}

	// Initialize player flying texture
	pTexture = loadTexture(ren, "player.png");
	player = new Player();

	player->CutAnimations();

	// menu control instructions
	stayInMenu = false;

	m = mainMenu(ren, font);
	if (m == 2) {
		gameIsRunning = false;
	}
	else if (m == 1) {
		stayInMenu = true;
	}
}

SDL_Texture *Game::loadTexture(SDL_Renderer *ren, char *name)
{
	SDL_Surface *image = IMG_Load(name);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, image);
	SDL_FreeSurface(image);

	return texture;
}

SDL_Texture *Game::loadText(SDL_Renderer *ren, const char *name, int fontsize)
{
	TTF_Font *font = TTF_OpenFont("Roboto-Black.ttf", fontsize); // Add font file to source directory
	SDL_Color colour = { 0, 255, 0 };
	SDL_Surface *textSurface = TTF_RenderText_Solid(font, name, colour);
	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(ren, textSurface);
	SDL_FreeSurface(textSurface);

	return textTexture;
}

// will display the game controls
bool Game::controls(SDL_Renderer *ren, bool stayInMenu)
{
	SDL_Event event;
	SDL_Texture *controlsDisplay = loadTexture(ren, "controls.png");
	SDL_Rect screen = { 0, 0, constants::SCREENWIDTH, constants::SCREENHEIGHT };
	while (stayInMenu) {
		SDL_RenderClear(ren);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
			{
				stayInMenu = false;
				break;
			}
			}
		}
		SDL_RenderCopy(ren, controlsDisplay, NULL, &screen);
		SDL_RenderPresent(ren);
	}
	SDL_DestroyTexture(controlsDisplay);
	return stayInMenu;
}

// Main menu for the game
int Game::mainMenu(SDL_Renderer *ren, TTF_Font *font)
{
	int x, y;
	const int NUMMENU = 3;
	const char *labels[NUMMENU] = { "Start", "Controls", "Exit" };
	SDL_Surface *menuSurfaces[NUMMENU];
	SDL_Texture *menuTextures[NUMMENU];
	bool selected[NUMMENU] = { false, false, false };
	SDL_Color color[2] = { { 255, 255, 255 },{ 255, 0, 0 } };
	SDL_Surface *screen = SDL_CreateRGBSurface(0, constants::SCREENWIDTH, constants::SCREENHEIGHT, 32, 0, 0, 0, 0);

	// Set menu surfaces and textures to their default values
	for (int i = 0; i < NUMMENU; i++) {
		menuSurfaces[i] = TTF_RenderText_Solid(font, labels[i], color[0]);
	}
	for (int i = 0; i < NUMMENU; i++) {
		menuTextures[i] = SDL_CreateTextureFromSurface(ren, menuSurfaces[i]);
	}

	// Create rectangles for the position and dimensions of the menu items
	SDL_Rect pos[NUMMENU];
	for (int i = 0; i < NUMMENU; i++) {
		pos[i].x = (screen->clip_rect.w / 2) - (menuSurfaces[0]->clip_rect.w / 2);
		pos[i].y = (screen->clip_rect.h / 2) - (menuSurfaces[0]->clip_rect.h) + 2 * i * (menuSurfaces[0]->clip_rect.h);
		pos[i].w = menuSurfaces[i]->clip_rect.w;
		pos[i].h = menuSurfaces[i]->clip_rect.h;
	}

	// Fill screen to be black for the background of the menu
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

	// Event handling declaration
	SDL_Event event;

	while (true)
	{
		//Input event handling
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// If you quit, free memory
			case SDL_QUIT:
			{
				for (int i = 0; i < NUMMENU; i++)
					SDL_FreeSurface(menuSurfaces[i]);
				return 2;
			}
			// Mouse movement
			case SDL_MOUSEMOTION:
			{
				// Get x and y of the mouse at all times
				x = event.motion.x;
				y = event.motion.y;

				// Check the collision detection of the mouse's X and Y on the menu items
				for (int i = 0; i < NUMMENU; i++)
				{
					if (x >= pos[i].x && x <= pos[i].x + pos[i].w && y >= pos[i].y && y <= pos[i].y + pos[i].h)
					{
						// If hovering over text, change state of the menu item
						if (!selected[i])
						{
							selected[i] = true;
							SDL_FreeSurface(menuSurfaces[i]);
							menuSurfaces[i] = TTF_RenderText_Solid(font, labels[i], color[1]);
							menuTextures[i] = SDL_CreateTextureFromSurface(ren, menuSurfaces[i]);
						}
					}
					else
					{
						// If the mouse moves out of the text, change the state of the menu item back to default
						if (selected[i])
						{
							selected[i] = false;
							SDL_FreeSurface(menuSurfaces[i]);
							menuSurfaces[i] = TTF_RenderText_Solid(font, labels[i], color[0]);
							menuTextures[i] = SDL_CreateTextureFromSurface(ren, menuSurfaces[i]);
						}
					}
				}
				break;
			}
			// Mouse button pressed
			case SDL_MOUSEBUTTONDOWN:
			{
				for (int i = 0; i < NUMMENU; i++)
				{
					if (selected[i])
					{
						Mix_PlayChannel(-1, sSelect, 0);
						// Free memory if menu text selected
						for (int j = 0; j < NUMMENU; j++)
							SDL_FreeSurface(menuSurfaces[j]);
						return i;
					}
				}
				break;
			}
			}
		}

		// Render menu text
		SDL_RenderClear(ren);
		for (int i = 0; i < NUMMENU; i++)
		{
			SDL_RenderCopy(ren, menuTextures[i], NULL, &pos[i]);
		}
		SDL_RenderPresent(ren);
	}
	Mix_FreeChunk(sSelect);
}

// Pause game
int Game::pauseGame(SDL_Renderer *ren, TTF_Font *font)
{
	int x, y;
	const int NUMMENU = 2;
	const char *labels[NUMMENU] = { "Resume", "Exit" };
	SDL_Surface *menuSurfaces[NUMMENU];
	SDL_Texture *menuTextures[NUMMENU];
	bool selected[NUMMENU] = { false, false };
	SDL_Color color[2] = { { 255, 255, 255 },{ 255, 0, 0 } };
	SDL_Surface *screen = SDL_CreateRGBSurface(0, constants::SCREENWIDTH, constants::SCREENHEIGHT, 32, 0, 0, 0, 0);

	// Set menu surfaces and textures to their default values
	for (int i = 0; i < NUMMENU; i++) {
		menuSurfaces[i] = TTF_RenderText_Solid(font, labels[i], color[0]);
	}
	for (int i = 0; i < NUMMENU; i++) {
		menuTextures[i] = SDL_CreateTextureFromSurface(ren, menuSurfaces[i]);
	}

	// Create rectangles for the position and dimensions of the menu items
	SDL_Rect pos[NUMMENU];
	for (int i = 0; i < NUMMENU; i++) {
		pos[i].x = (screen->clip_rect.w / 2) - (menuSurfaces[0]->clip_rect.w / 2);
		pos[i].y = (screen->clip_rect.h / 2) - (menuSurfaces[0]->clip_rect.h) + 2 * i * (menuSurfaces[0]->clip_rect.h);
		pos[i].w = menuSurfaces[i]->clip_rect.w;
		pos[i].h = menuSurfaces[i]->clip_rect.h;
	}

	// Fill screen to be black for the background of the menu
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));

	// Event handling declaration
	SDL_Event event;

	while (true)
	{
		Mix_PauseMusic();
		//Input event handling
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				// If you quit, free memory
			case SDL_QUIT:
			{
				for (int i = 0; i < NUMMENU; i++)
					SDL_FreeSurface(menuSurfaces[i]);
				return 1;
			}
			// Mouse movement
			case SDL_MOUSEMOTION:
			{
				// Get x and y of the mouse at all times
				x = event.motion.x;
				y = event.motion.y;

				// Check the collision detection of the mouse's X and Y on the menu items
				for (int i = 0; i < NUMMENU; i++)
				{
					if (x >= pos[i].x && x <= pos[i].x + pos[i].w && y >= pos[i].y && y <= pos[i].y + pos[i].h)
					{
						// If hovering over text, change state of the menu item
						if (!selected[i])
						{
							selected[i] = true;
							SDL_FreeSurface(menuSurfaces[i]);
							menuSurfaces[i] = TTF_RenderText_Solid(font, labels[i], color[1]);
							menuTextures[i] = SDL_CreateTextureFromSurface(ren, menuSurfaces[i]);
						}
					}
					else
					{
						// If the mouse moves out of the text, change the state of the menu item back to default
						if (selected[i])
						{
							selected[i] = false;
							SDL_FreeSurface(menuSurfaces[i]);
							menuSurfaces[i] = TTF_RenderText_Solid(font, labels[i], color[0]);
							menuTextures[i] = SDL_CreateTextureFromSurface(ren, menuSurfaces[i]);
						}
					}
				}
				break;
			}
			// Mouse button pressed
			case SDL_MOUSEBUTTONDOWN:
			{
				for (int i = 0; i < NUMMENU; i++)
				{
					if (selected[i])
					{
						Mix_PlayChannel(-1, sSelect, 0);
						Mix_ResumeMusic();
						// Free memory if menu text selected
						for (int j = 0; j < NUMMENU; j++)
							SDL_FreeSurface(menuSurfaces[j]);
						return i;
					}
				}
				break;
			}
			// Key press escape
			case SDL_KEYDOWN:
			{
				// If escape is pressed on menu, continue to game loop
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					for (int i = 0; i < NUMMENU; i++)
						SDL_FreeSurface(menuSurfaces[i]);
					Mix_ResumeMusic();
					return 0;
				}
				break;
			}
			}
		}

		// Render menu text
		SDL_RenderClear(ren);
		for (int i = 0; i < NUMMENU; i++)
		{
			SDL_RenderCopy(ren, menuTextures[i], NULL, &pos[i]);
		}
		SDL_RenderPresent(ren);
	}
	Mix_FreeChunk(sSelect);
}

void Game::capFrameRate()
{
	fpsTimer.start();
	capTimer.start();
	avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);

	if (avgFPS > 2000000)
	{
		avgFPS = 0;
	}

	++countedFrames;

	//If frame finished early
	frameTicks = capTimer.getTicks();
	if (frameTicks < constants::SCREEN_TICK_PER_FRAME)
	{
		//Wait remaining time
		SDL_Delay(constants::SCREEN_TICK_PER_FRAME - frameTicks);
	}
}

void Game::handlePlayerInputs()
{
	// Gets state of keypress directly from keyboard instead of polling events. waaay better movement response
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_A])  // left
	{
		player->playerStraight = false;
		player->playerRight = false;
		player->playerLeft = true;
		player->moveLeft();
	}
	else if (keys[SDL_SCANCODE_D]) // right
	{
		player->playerStraight = false;
		player->playerLeft = false;
		player->playerRight = true;
		player->moveRight();
	}
	else
	{
		player->playerStraight = true;
		player->playerLeft = false;
		player->playerRight = false;
	}
	if (keys[SDL_SCANCODE_S]) // down
	{
		player->playerStraight = true;
		player->playerLeft = false;
		player->playerRight = false;
		player->moveDown();
	}
	if (keys[SDL_SCANCODE_W]) // up
	{
		player->playerStraight = true;
		player->playerLeft = false;
		player->playerRight = false;
		player->moveUp();
	}
	if (keys[SDL_SCANCODE_SPACE]) // Shoot
	{
		if (bullet == nullptr)
		{
			Mix_PlayChannel(2, sShoot, 0);
			player->playerShoot = true;
			bullet = new Bullet(player->rect.x + 16, player->rect.y);
		}
	}
	if (keys[SDL_SCANCODE_ESCAPE]) // Pause
	{
		p = pauseGame(ren, font);
		if (p == 1)
			gameIsRunning = false;
	}
	SDL_PollEvent(&events);
	if (SDL_PollEvent(&events))
	{
		switch (events.type)
		{
		case SDL_QUIT:
			gameIsRunning = false;
			break;
		}
	}

	// Player shoot
	if (player->playerShoot)
	{
		// move bullet
		if (bullet != nullptr) {
			bullet->moveStep();
			if (bullet->rect.y <= 0) {
				delete bullet;
				bullet = nullptr;
			}
		}
	}
}

void Game::updateGame()
{
	// Controls menu
	if (stayInMenu) {
		stayInMenu = controls(ren, stayInMenu);
	}

	// Scroll background
	++scrollingOffset;
	if (scrollingOffset / 3 > constants::SCREENHEIGHT)
	{
		scrollingOffset = 0;
	}

	// Set rects for scrolling background
	renderQuad = { 0, int(scrollingOffset / 3.0f), int(constants::SCREENWIDTH), int(constants::SCREENHEIGHT) };
	renderQuad1 = { 0, int(scrollingOffset / 3.0f - constants::SCREENHEIGHT), int(constants::SCREENWIDTH), int(constants::SCREENHEIGHT) };

	// Set player animation
	player->SetAnimation();

	// Moves enemies
		for (int i = 0; i < numEnemies; i++)
			enemies[i].moveStraight();


	// if Enemy-bullet collision, respawn enemies and add points
	for (int i = 0; i < numEnemies; i++) {
		if (SDL_HasIntersection(&enemies[i].rect, &bullet->rect)) {
			enemies[i].hit = true;
			enemies[i].expFrame = 0; // start explosion animation
			explosionRect = { enemies[i].rect.x, enemies[i].rect.y, 100, 100 }; // get position of explosion
			points++;
			Mix_PlayChannel(-1, sExplode, 0);
			delete bullet;
			bullet = nullptr;
			enemies[i].Respawn();
		}
		// If enemy goes off screen, respawn and lose a life
		if (enemies[i].rect.y > constants::SCREENHEIGHT) {
			enemies[i].Respawn();
			lives--;
			ssLives.str(std::string());
			ssLives << "Lives: " << lives;
			life = ssLives.str();
			UILives = loadText(ren, life.c_str(), 20);
		}
		// If enemy collides with player, respawn enemy and lose a life
		else if (SDL_HasIntersection(&enemies[i].rect, &player->rect)) {
			enemies[i].hit = true;
			enemies[i].expFrame = 0;
			explosionRect = { (enemies[i].rect.x + player->rect.x) / 2, (enemies[i].rect.y + player->rect.y) / 2, 100, 100 }; // position of explosion
			Mix_PlayChannel(0, sExplode, 0);
			lives--;
			ssLives.str(std::string());
			ssLives << "Lives: " << lives;
			life = ssLives.str();
			UILives = loadText(ren, life.c_str(), 20);
			enemies[i].Respawn();
		}
	}

	// If we have no lives, go to gameover screen and then restart game
	if (lives <= 0) {
		// If we got highscore, update and save it
		if (points > highscore) {
			highscore = points;
			pHighScore->SetText(highscore);
			doc.SaveFile("gameData.xml");
			ssHighScore.str(std::string()); // clear stringstream of old variables
			ssHighScore << "HighScore: " << highscore; // input current score in stringstream
			highScore = ssHighScore.str();
			UIHighScore = loadText(ren, highScore.c_str(), 16);
		}
		UIScore = loadText(ren, "Score: 0", 18);
		UILives = loadText(ren, "Lives: 3", 18);
		SDL_RenderCopy(ren, gameover, NULL, &gameoverRect);
		SDL_RenderPresent(ren);
		Mix_PauseMusic();
		SDL_Delay(5000);
		Mix_PlayChannel(-1, sExplode, 0);
		Mix_RewindMusic();
		Mix_ResumeMusic();
		for (int i = 0; i < numEnemies; i++) {
			enemies[i].Respawn();
		}
		lives = 3;
		points = 0;
	}

	// Makes sure enemies don't respawn on top of each other
	for (int i = 0; i < (numEnemies - 1); i++) {
		for (int j = (i + 1); j < numEnemies; j++) {
			if (SDL_HasIntersection(&enemies[i].rect, &enemies[j].rect))
			{
				enemies[i].Respawn();
			}
		}
	}
}

void Game::renderScene()
{
	SDL_RenderClear(ren);

	//Render background
	SDL_RenderCopy(ren, background, NULL, &renderQuad);
	SDL_RenderCopy(ren, background, NULL, &renderQuad1);

	// Render bullet
	if (bullet != nullptr)
		SDL_RenderCopy(ren, bulletTexture, NULL, &bullet->rect);

	// Render enemies and explosion
	for (int i = 0; i < numEnemies; i++) {
		enemies[i].render(ren, enemyTexture);
		if (enemies[i].expFrame < 30) // Only render 1 full animation
			enemies[i].renderExplosion(ren, enemyExplosion, explosionRect);
	}

	// Render player
	player->render(ren, pTexture);

	// Render score, lives, and highscore
	ssScore.str(std::string()); // clear stringstream of old variables
	ssScore << "Score: " << points; // input current score in stringstream
	score = ssScore.str();
	UIScore = loadText(ren, score.c_str(), 20);

	SDL_RenderCopy(ren, UIScore, NULL, &UIScoreRect);
	SDL_RenderCopy(ren, UILives, NULL, &UILivesRect);
	SDL_RenderCopy(ren, UIHighScore, NULL, &UIHighScoreRect);

	// render
	SDL_RenderPresent(ren);
}

// Game Loop
void Game::runGame()
{	
	srand(time(NULL));
	while (gameIsRunning)
	{
		capFrameRate();
		handlePlayerInputs();
		updateGame();
		renderScene();
	}
}

void Game::destroy()
{
	// Free memory
	delete[] enemies;
	delete player;
	delete bullet;
	SDL_DestroyTexture(bulletTexture);
	SDL_DestroyTexture(pTexture);
	SDL_DestroyTexture(enemyTexture);
	SDL_DestroyTexture(enemyExplosion);
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(UIScore);
	SDL_DestroyTexture(UILives);
	SDL_DestroyTexture(UIHighScore);
	Mix_FreeChunk(sShoot);
	Mix_FreeChunk(sExplode);
	Mix_FreeMusic(bMusic);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	win = NULL;
	ren = NULL;

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

