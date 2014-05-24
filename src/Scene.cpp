#include "Scene.h"


Scene::Scene()
{
}


Scene::~Scene()
{
}

std::string & 
Scene::GetName()
{
	return name;
}

void 
Scene::SetName(const std::string & name)
{
	this->name = name;
}

void
Scene::Init(SDL_Renderer * renderer)
{

}

void 
Scene::Render(SDL_Renderer * renderer)
{

}

void
Scene::Update(float seconds)
{

}

void 
Scene::OnEvent(SDL_Event & ev)
{

}
