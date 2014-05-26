#include "IntroScene.h"

IntroScene::~IntroScene()
{
	SDL_DestroyTexture(splash);
}

void
IntroScene::Init(SDL_Renderer * renderer)
{
	splash = IMG_LoadTexture(renderer, "res/splash.bmp");
	if (splash == NULL) throw std::runtime_error(IMG_GetError());		
}

void
IntroScene::Render(SDL_Renderer * renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, splash, NULL, NULL);
	
	// fade out effect
	if (fading)
	{
		int width = 0, height = 0;
		SDL_GetWindowSize(Game::GetInstance()->GetWindow(), &width, &height);
		boxRGBA(renderer, 0, 0, width, height, 0, 0, 0, (int)alpha);
	}
}

// use this to show rendered texture for certain time
void
IntroScene::Update(float seconds)
{
	const float ALPHA_VALUE_STEP = 128.0f;
	if (fading)
	{
		alpha += seconds * ALPHA_VALUE_STEP;
		if (alpha >= 255.0f)
			Game::GetInstance()->SetCurrentScene(Game::GetInstance()->FindScene("Game"));
	}	
}

void
IntroScene::OnEvent(SDL_Event & ev)
{
	switch (ev.type)
	{
	case SDL_QUIT:
		Game::GetInstance()->SetProperty("running", false);
		Mix_FadeOutMusic(3000);
		SDL_Delay(3500);
		break;
	case SDL_KEYDOWN:
		fading = true;
		break;
	}
}

void
IntroScene::OnEnter()
{
	Mix_FadeInMusic(Game::GetInstance()->music["background"], -1, 3000);
}

void
IntroScene::OnExit()
{

}