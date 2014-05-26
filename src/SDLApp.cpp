#include "SDLApp.h"
#include "Game.h"
#include "Commands.h"
#include "IntroScene.h"
#include "GameScene.h"
////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////
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

	// initialize mixer settings
	int mixerFlags = MIX_INIT_MP3;
	int returnedFlags = Mix_Init(mixerFlags);
	if ((mixerFlags & returnedFlags) != mixerFlags)
	{
		throw runtime_error(Mix_GetError());
	}

	int freq = 44100;
	Uint16 format = AUDIO_S16SYS;
	int channels = 2;
	int buffer = 1024;

	if (Mix_OpenAudio(freq, format, channels, buffer))
	{
		throw runtime_error(Mix_GetError());
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
SDLApp::~SDLApp()
{

	if (window) SDL_DestroyWindow(window);
	if (renderer) SDL_DestroyRenderer(renderer);

	for (auto & it : scenes)
	{
		delete it.second;
	}

	for (auto & it : music)
	{
		Mix_FreeMusic(it.second);
	}
	music.clear();

	for (auto & it : sounds)
	{
		Mix_FreeChunk(it.second);
	}
	sounds.clear();
	
	Mix_CloseAudio();
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();	
}
////////////////////////////////////////////////////////////////////////////////////////////
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

	Mix_Music * song = Mix_LoadMUS("res/audio/background.mp3");
	if (song == NULL)
	{
		throw runtime_error(Mix_GetError());
	}
	music["background"] = song;

	Mix_Chunk * tmp = Mix_LoadWAV("res/audio/step.wav");
	if (tmp == NULL) throw runtime_error(Mix_GetError());
	sounds["step"] = tmp;

	tmp = Mix_LoadWAV("res/audio/blocked.wav");
	if (tmp == NULL) throw runtime_error(Mix_GetError());
	sounds["blocked"] = tmp;

	tmp = Mix_LoadWAV("res/audio/death.wav");
	if (tmp == NULL) throw runtime_error(Mix_GetError());
	sounds["death"] = tmp;

	tmp = Mix_LoadWAV("res/audio/squish.wav");
	if (tmp == NULL) throw runtime_error(Mix_GetError());
	sounds["squish"] = tmp;
		
	IntroScene * intro = new IntroScene();
	intro->Init(renderer);
	intro->SetName("Intro");
	AddScene(intro);
	
	GameScene * game = new GameScene();
	game->Init(renderer);
	game->SetName("Game");
	AddScene(game);
		
	SetCurrentScene(intro);
	page = game->page;
	time = SDL_GetTicks();
	
}
////////////////////////////////////////////////////////////////////////////////////////////
void
SDLApp::Render(SDL_Renderer * renderer)
{
	if (currentScene) currentScene->Render(renderer);
	// draw all texture changes made in current scene
	SDL_RenderPresent(renderer);
}
////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////
void
SDLApp::SetCurrentScene(Scene * scene)
{
	auto it = scenes.find(scene->GetName());
	if (it != scenes.end())
	{
		// call exit from previous scene
		if (currentScene != NULL)
			currentScene->OnExit();

		// change to new scene
		currentScene = it->second;
		// call enter for new scene
		currentScene->OnEnter();
	}
	else
	{
		ostringstream ss("Cannot set current scene. Scene with a given name ");
		ss << scene->GetName() << " doesn't exists.";
		throw runtime_error(ss.str());
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
Scene * 
SDLApp::GetCurrentScene()
{
	return currentScene;
}
////////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////
SDL_Window * 
SDLApp::GetWindow()
{
	return window;
}
////////////////////////////////////////////////////////////////////////////////////////////
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
