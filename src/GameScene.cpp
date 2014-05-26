#include "GameScene.h"
#include "Game.h"
#include <stdexcept>
#include "SDL2_gfxPrimitives.h"
#include "SDL_ttf.h"
#include "tinyxml2.h"
#include <map>
#include <sstream>
#include "globals.h"
#include "Room.h"
////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace tinyxml2;
extern map<XMLError, string> g_TinyXmlErrors;
////////////////////////////////////////////////////////////////////////////////////////////
void
GameScene::Init(SDL_Renderer * renderer)
{
	bookCover = IMG_LoadTexture(renderer, "res/cover.png");
	if (bookCover == NULL) throw std::runtime_error(IMG_GetError());

	bookPages = IMG_LoadTexture(renderer, "res/pages.png");
	if (bookPages == NULL) throw std::runtime_error(IMG_GetError());

	playerTexture = IMG_LoadTexture(renderer, "res/Player0.png");
	if (playerTexture == NULL) throw std::runtime_error(IMG_GetError());

	// get width and height pixel values from player texture
	// and put them as player's rectangle values
	int playerWidth = 0, playerHeight = 0; 
	// get player width and height from player texture
	SDL_QueryTexture(playerTexture, NULL, NULL, &playerWidth, &playerHeight);
	// get window's width and height
	// NOT WORKING ATM. 
	// FUNCTION KEEP CALLING ITSELF INFINITELY CREATING A NEW WINDOW EVERYTIME
	//SDL_GetWindowSize(Game::GetInstance()->GetWindow(), &playerRect.x, &playerRect.y);
	// place player at center of the screen
	playerRect.x = 640 / 2;
	playerRect.y = 480 / 2;
	playerRect.w = playerWidth * 2;
	playerRect.h = playerHeight * 2;
	
	// declare a "page" where to put text
	page = new Page();
	page->fillMode_ = FillMode::Tail;
	page->_justification = Justification::Left;
	page->Init(renderer, 412, 612);
	*page << "Hello there";
	page->SetDirty(true);

	// load fonts
	XMLDocument fontDoc;
	fontDoc.LoadFile("res/fonts/fonts.xml");
	if (fontDoc.ErrorID() != XML_NO_ERROR)
	{
		ostringstream ss;
		ss << "Font load file:" 
			<< g_TinyXmlErrors[fontDoc.ErrorID()] 
			<< " " << fontDoc.GetErrorStr1() 
			<< " " << fontDoc.GetErrorStr2();
		throw XmlParsingException(ss.str());
	}

	XMLElement * pElement = fontDoc.FirstChildElement("Fonts");
	if (!pElement) throw XmlParsingException("No fonts declaration found.");

	pElement = pElement->FirstChildElement("Font");
	while (pElement != NULL)
	{
		const char * name = pElement->Attribute("name");
		const char * size = pElement->Attribute("size");
		const char * path = pElement->Attribute("path");
		if (!name) throw AttributeMissingException("Name missing from font");
		if (!size) throw AttributeMissingException("Size missing from font");
		if (!path) throw AttributeMissingException("Path missing from font");

		TTF_Font * tmp = NULL;
		stringstream ss(size);
		int fontSize;
		if (!(ss >> fontSize)) throw runtime_error("Cannot convert font size to int");
		tmp = TTF_OpenFont(path, fontSize);
		if (tmp == NULL) throw runtime_error(TTF_GetError());
		fonts[name] = tmp;

		pElement = pElement->NextSiblingElement("Font");
	}
}
////////////////////////////////////////////////////////////////////////////////////////////
GameScene::~GameScene()
{
	SDL_DestroyTexture(bookCover);
	SDL_DestroyTexture(bookPages);
	SDL_DestroyTexture(playerTexture);	
}
////////////////////////////////////////////////////////////////////////////////////////////
void
GameScene::Render(SDL_Renderer * renderer)
{
	if (bookCoverDisplayed == false)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bookCover, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_Delay(1000);
		bookCoverDisplayed = true;		
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bookPages, NULL, NULL);
		SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);		
		
		if (page->IsDirty())
		{
			page->Compose(fonts["text"]);
			page->RenderContent(renderer);
			page->SetDirty(false);
		}
		SDL_Rect d = { 50, 50, 320, 400 };
		SDL_RenderCopy(renderer, page->_pageTexture, NULL, &d);
		
		if (fading)
		{
			// cover whole window with white color
			int width = 0, height = 0;
			SDL_GetWindowSize(Game::GetInstance()->GetWindow(), &width, &height);
			boxRGBA(renderer, 0, 0, width, height, 255, 255, 255, (int)alpha);
		}		
	}
}
////////////////////////////////////////////////////////////////////////////////
// use this to show rendered texture for certain time
void
GameScene::Update(float seconds)
{
	const float ALPHA_VALUE_STEP = 64.0f;
	if (fading)
	{
		alpha -= seconds * ALPHA_VALUE_STEP;
		if (alpha < 0.001f) fading = false;
	}
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::OnEvent(SDL_Event & ev)
{
	Command *pCmd = CommandUtils::Parse(ev);
	pCmd->Execute(*this);
	delete pCmd;	
}
////////////////////////////////////////////////////////////////////////////////
void 
GameScene::OnEnter()
{

}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::OnExit()
{

}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(QuitCommand & cmd)
{
	Game::GetInstance()->SetProperty("running", false);
	Mix_FadeOutMusic(3000);
	SDL_Delay(3500);
	SDL_Quit();
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(MoveCommand & cmd)
{
	if (cmd.m_Dir == kNumDirs)
	{
		*page << "You want to move ... where?\n";
		page->SetDirty(true);
	}
	else
	{
		Room *pNext = Game::GetInstance()->GetCurrentRoom()->GetNextRoom(cmd.m_Dir);
		if (pNext)
		{
			// Handle move commands in both rooms
			Game::GetInstance()->GetCurrentRoom()->Execute(cmd);
			Game::GetInstance()->SetCurrentRoom(pNext);
			Game::GetInstance()->GetCurrentRoom()->Execute(cmd);
			// move player character on screen
			switch (cmd.m_Dir)
			{
			case North:
				playerRect.y -= playerRect.h;
				break;
			case South:
				playerRect.y += playerRect.h;
				break;
			case East:
				playerRect.x += playerRect.w;
				break;
			case West:
				playerRect.x -= playerRect.w;
				break;
			}
			// play sound sample
			Mix_PlayChannel(-1, Game::GetInstance()->sounds["step"], 0);
		}
		else
		{
			*page << "You bump your head on the wall. You can't go that way.\n";
			page->SetDirty(true);
			// play sound sample
			Mix_PlayChannel(-1, Game::GetInstance()->sounds["blocked"], 0);
		}
	}
	page->SetDirty(true);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(UnknownCommand & cmd)
{
	*page << "Sorry, I did not quite get that.\n";
	page->SetDirty(true);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(TakeCommand & cmd)
{
	Game::GetInstance()->GetCurrentRoom()->Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(DropCommand & cmd)
{
	Game::GetInstance()->GetCurrentRoom()->Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(InventoryCommand & cmd)
{
	Game::GetInstance()->GetPlayer().Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(LookCommand & cmd)
{
	Game::GetInstance()->GetCurrentRoom()->Execute(cmd);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(ExamineCommand & cmd)
{
	try
	{
		Game::GetInstance()->GetPlayer().Execute(cmd);
	}
	catch (ExamineCommandFailOnPlayerException & ex)
	{
		Game::GetInstance()->GetCurrentRoom()->Execute(cmd);
	}

}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(UseCommand & cmd)
{
	// try player's inventory use first
	try {
		Game::GetInstance()->GetPlayer().Execute(cmd);
	}
	catch (UseCommandFailOnPlayerException & ex)
	{
		// no go, try from room instead.
		Game::GetInstance()->GetCurrentRoom()->Execute(cmd);
	}
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::Execute(NullCommand & cmd)
{

}

