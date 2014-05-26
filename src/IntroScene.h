#ifndef __IntroScene_h__
#define __IntroScene_h__

#include <stdexcept>
#include "Scene.h"
#include "SDL_Image.h"
#include "Game.h"
#include "SDL2_gfxPrimitives.h"

class IntroScene : public Scene
{
protected:
	SDL_Texture *	splash{ NULL };
	float			alpha{ 0.0f };
	bool			fading{ false };
public:
	virtual ~IntroScene();
	virtual void Init(SDL_Renderer * renderer);
	virtual void Render(SDL_Renderer * renderer);
	virtual void Update(float seconds);
	virtual void OnEvent(SDL_Event & ev); 
	virtual void OnEnter();
	virtual void OnExit();
};

#endif