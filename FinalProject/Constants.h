#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "tinyxml2.h"
#include <string>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

namespace constants
{
	const int SCREENWIDTH = 800;
	const int SCREENHEIGHT = 600;
	const int SCREEN_FPS = 60;
	const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;
	const int PLAYERSPEED = 6;
	const int BULLETSPEED = 13;
	const int ENEMYSPEED = 2;
}

#endif
