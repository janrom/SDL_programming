#ifndef __GameScene_h__
#define __GameScene_h__

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "Scene.h"
#include "Page.h"
#include "Commands.h"
#include <map>
#include <string>
const int NUM_PAGE_ANIM_FRAMES = 6;
const int TEXT_MAX_WIDTH = 979;
class GameScene :	public Scene,
					public NOPCommandHandler
{
protected:
	SDL_Texture *	bookCover{ NULL };
	SDL_Texture *	bookPages[NUM_PAGE_ANIM_FRAMES];
	SDL_Texture *	playerTexture{ NULL };
	SDL_Rect		playerRect;
	SDL_Rect		textSrc;
	float			alpha{ 255.0f };
	float			pageFrame{ 0 };
	int				pageIndex{ 0 };
	bool			fading{ true };
	bool			bookCoverDisplayed{ false };
	bool			nextPageTurning{ false };
	bool			previousPageTurning{ false };
	std::map<std::string, TTF_Font *> fonts;	

public:
	Page *			page{ NULL };
	Page *			titlePage{ NULL };
	Page *			endPage{ NULL };
	virtual void Init(SDL_Renderer * renderer);
	virtual ~GameScene();
	virtual void Update(float seconds);
	virtual void Render(SDL_Renderer * renderer);	
	virtual void OnEvent(SDL_Event & ev);
	virtual void OnEnter();
	virtual void OnExit();

	void Execute(UseCommand & cmd);
	void Execute(ExamineCommand & cmd);
	void Execute(QuitCommand & cmd);
	void Execute(MoveCommand & cmd);
	void Execute(UnknownCommand & cmd);
	void Execute(TakeCommand & cmd);
	void Execute(DropCommand & cmd);
	void Execute(InventoryCommand & cmd);
	void Execute(LookCommand & cmd);
	void Execute(NullCommand & cmd);
};

#endif