// Copyright anssi.grohn@karelia.fi (c) 2014.
// Licensed under GPLv3.
#ifndef __Player_h__
#define __Player_h__
////////////////////////////////////////////////////////////////////////////////
#include <GameObject.h>
#include <tinyxml2.h>
#include <Commands.h>
#include "SDL.h"
////////////////////////////////////////////////////////////////////////////////
class Player : public GameObject,
	       public NOPCommandHandler
{
private:
	SDL_Rect sdl_rect;
public:
	Player();
  void Save( tinyxml2::XMLPrinter & p);
  void Execute( UseCommand & cmd );
  void Execute( InventoryCommand & cmd );
  void Execute( ExamineCommand & cmd );
  
  // functions for placing and moving the player image
  SDL_Rect * GetRect() { return &sdl_rect; }
  int & GetHeight() { return sdl_rect.h; }
  int & GetWidth() { return sdl_rect.w; }
  int & GetX() { return sdl_rect.x; }
  int & GetY() { return sdl_rect.y; }
  void SetHeight(const int & h) { sdl_rect.h = h; }
  void SetWidth(const int & w) { sdl_rect.w = w; }
  void SetX(const int & x) { sdl_rect.x = x; }
  void SetY(const int & y) { sdl_rect.y = y; }
  void IncreaseX() { sdl_rect.x += GetWidth(); }
  void DecreaseX() { sdl_rect.x -= GetWidth(); }
  void IncreaseY() { sdl_rect.y += GetHeight(); }
  void DecreaseY() { sdl_rect.y -= GetHeight(); }
};
////////////////////////////////////////////////////////////////////////////////
#endif
