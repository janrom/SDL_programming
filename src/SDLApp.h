#ifndef __SDLApp_h__
#define __SDLApp_h__

#include "SDL.h"
#include "Command.h"
#include <string>
#include <vector>

class SDLApp
{
protected:
	SDL_Window * window;
	SDL_Renderer * renderer;		
public:
	SDLApp();
	virtual ~SDLApp();
	void Init(const std::string & title,
		int width,
		int height,
		int flags = SDL_WINDOW_SHOWN);
	void Render(SDL_Texture * texture);
	void Render(std::vector<SDL_Texture *> & textures, std::vector<SDL_Rect *> & srcRects, std::vector<SDL_Rect *> & dstRects);
	void HandleInput();	
};

#endif