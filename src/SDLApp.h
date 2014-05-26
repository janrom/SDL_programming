#ifndef __SDLApp_h__
#define __SDLApp_h__

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <sstream>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "Scene.h"
#include "Page.h"

class SDLApp
{
protected:
	SDL_Window *	window;
	SDL_Renderer *	renderer;
	Scene *			currentScene;	
	Uint32			time;
	std::map<std::string, Scene *> scenes;
public:
	std::map<std::string, Mix_Music *> music;
	std::map<std::string, Mix_Chunk *> sounds;
	Page *	page;
	SDLApp();
	virtual ~SDLApp();	
	void Init(const std::string & title,
		int width,
		int height,
		int flags = SDL_WINDOW_SHOWN);
	// TODO: move texture handling away from Game-class
	void Render(SDL_Renderer * renderer);
	void Update();
	void HandleInput();	

	// for Scene-class handling
	void AddScene(Scene * scene);
	void DeleteScene(Scene * scene);
	void SetCurrentScene(Scene * scene);
	Scene * GetCurrentScene();
	Scene * FindScene( std::string name );

	SDL_Window * GetWindow();	
};

#endif
