// Copyright anssi.grohn@karelia.fi (c) 2014.
// Licensed under GPLv3.
#include "Game.h"
#include "SDL.h"
#include <iostream>
#include <string>
#include <stdexcept>
////////////////////////////////////////////////////////////////////////////////
using namespace std;
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		throw runtime_error(SDL_GetError());
	}
	
	Game & g = *Game::GetInstance();
	g.Play();
	
	SDL_Quit();

	return 0;
}
////////////////////////////////////////////////////////////////////////////////
