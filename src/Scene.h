#pragma once

#include "SDL.h"
#include "Updateable.h"
#include "PropertyHolder.h"
#include <string>

class Scene :	public Updateable,
				public PropertyHolder
{
protected:
	std::string name;
public:
	Scene();
	virtual ~Scene();

	std::string & GetName();
	void SetName(const std::string & name);
	virtual void Init(SDL_Renderer * renderer);
	virtual void Render(SDL_Renderer * renderer);
	virtual void Update(float seconds);
	virtual void OnEvent(SDL_Event & ev);
	virtual void OnEnter();
	virtual void OnExit();
};
