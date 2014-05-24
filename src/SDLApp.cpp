#include "SDLApp.h"
#include "Game.h"
#include "Commands.h"
#include "IntroScene.h"
#include "GameScene.h"

using namespace std;

SDLApp::SDLApp()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		throw runtime_error(SDL_GetError());
	}
	window = NULL;
	renderer = NULL;	
	currentScene = NULL;
	
	if (TTF_Init()) throw runtime_error(TTF_GetError());
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

	renderer = SDL_CreateRenderer(	window, -1, 
									SDL_RENDERER_ACCELERATED |
									SDL_RENDERER_TARGETTEXTURE );

	if (renderer == NULL) throw runtime_error(SDL_GetError());	

	time = SDL_GetTicks();

	IntroScene * intro = new IntroScene();
	intro->Init(renderer);
	intro->SetName("Intro");
	AddScene(intro);
	SetCurrentScene(intro);

	GameScene * game = new GameScene();
	game->Init(renderer);
	game->SetName("Game");
	AddScene(game);

	page = game->page;
}

void
SDLApp::Render(SDL_Renderer * renderer)
{
	if (currentScene) currentScene->Render(renderer);
	// draw all texture changes made in current scene
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
			currentScene->OnEvent(ev);			
		break;				
		}
	}
}

void 
SDLApp::AddScene(Scene * scene)
{
	if (scenes.find(scene->GetName()) != scenes.end())
	{
		ostringstream ss("The given name");
		ss << scene->GetName() << "is already in use";
		throw runtime_error(ss.str());
	}
	scenes[scene->GetName()] = scene;
}

void
SDLApp::DeleteScene(Scene * scene)
{
	auto it = scenes.find(scene->GetName());
	if (it != scenes.end())
	{
		delete it->second;
		scenes.erase(it);
	}
	else
	{
		ostringstream ss("Cannot delete a scene. Scene with a given name ");
		ss << scene->GetName() << " doesn't exists.";
		throw runtime_error(ss.str());
	}
}

void
SDLApp::SetCurrentScene(Scene * scene)
{
	auto it = scenes.find(scene->GetName());
	if (it != scenes.end())
	{
		currentScene = it->second;
	}
	else
	{
		ostringstream ss("Cannot set current scene. Scene with a given name ");
		ss << scene->GetName() << " doesn't exists.";
		throw runtime_error(ss.str());
	}
}

Scene * 
SDLApp::GetCurrentScene()
{
	return currentScene;
}

Scene *
SDLApp::FindScene(string name)
{
	auto it = scenes.find(name);
	if (it != scenes.end())
	{
		return it->second;
	}
	else
	{
		ostringstream oss("Cannot find a scene with given name: ");
		oss << name;
		throw runtime_error(oss.str());
	}
}

SDL_Window * 
SDLApp::GetWindow()
{
	return window;
}

void
SDLApp::Update()
{
	const int MIN_ALLOWED_TIME_STEP_MS = 5;
	Uint32 slice = SDL_GetTicks() - time;
	if (slice >= MIN_ALLOWED_TIME_STEP_MS)
	{
		if (currentScene)
		{
			float seconds = (float)slice * 0.001f;
			currentScene->Update(seconds);
			time = SDL_GetTicks();
		}
	}	
}