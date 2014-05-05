#include "SDL.h"
#include "SDLApp.h"
#include "SDL_image.h"
#include <stdexcept>
#include <string>
#include "Game.h"

using namespace std;

SDLApp::SDLApp()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		throw runtime_error(SDL_GetError());
	}
	window = NULL;
	renderer = NULL;	
}

SDLApp::~SDLApp()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();	
}

void 
SDLApp::Init(const std::string & title, int width, int height, int flags)
{
	// create SDL window
	window = SDL_CreateWindow(title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		flags);

	// throw error if window was not created
	if (window == NULL) throw runtime_error(SDL_GetError());

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) throw runtime_error(SDL_GetError());	
}

void
SDLApp::Render(SDL_Texture * texture)
{
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void 
SDLApp::Render(std::vector<SDL_Texture *> & textures, std::vector<SDL_Rect *> & srcRects, std::vector<SDL_Rect *> & dstRects)
{
	// render images
	int index = 0;
	for (auto it = textures.begin(); it != textures.end(); it++)
	{
		SDL_RenderCopy(renderer, textures[index], srcRects[index], dstRects[index]);
		index++;
	}
	SDL_RenderPresent(renderer);	
}

void
SDLApp::HandleInput()
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
		case SDL_QUIT:
		case SDL_KEYDOWN:
			Command *pCmd = CommandUtils::Parse(ev);
			pCmd->Execute(*Game::GetInstance());
			delete pCmd;
		break;				
		}
	}
}