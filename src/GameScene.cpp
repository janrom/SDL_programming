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
	bookCover = IMG_LoadTexture(renderer, "res/pages/cover.png");
	if (bookCover == NULL) throw std::runtime_error(IMG_GetError());

	for (int i = 0; i < NUM_PAGE_ANIM_FRAMES; i++)
	{
		ostringstream oss;
		oss << "res/pages/page0" << i + 1 << ".png";
		bookPages[i] = IMG_LoadTexture(renderer, oss.str().c_str());
		if (bookPages[i] == NULL) throw std::runtime_error(IMG_GetError());
	}	

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
	playerRect.x = 480 - playerWidth;
	playerRect.y = 240 - playerHeight;
	playerRect.w = playerWidth * 2;
	playerRect.h = playerHeight * 2;
			
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

	// declare a basic page where to put text
	page = new Page();
	page->fillMode_ = FillMode::Tail;
	page->_justification = Justification::Left;
	page->Init(renderer, 412, 612);
	page->SetDirty(true);

	titlePage = new Page();
	titlePage->fillMode_ = FillMode::Head;
	titlePage->_justification = Justification::Center;
	titlePage->Init(renderer, 412, 612);
	*titlePage << "Quick Escape by Janne Romppanen";
	titlePage->SetDirty(true);

	endPage = new Page();
	endPage->fillMode_ = FillMode::Head;
	endPage->_justification = Justification::Center;
	endPage->Init(renderer, 412, 612);
	*endPage << "The End";
	endPage->SetDirty(true);

	// default size for a text area
	textSrc.x = 0;
	textSrc.y = 0;
	textSrc.w = 979;
	textSrc.h = 626;
}
////////////////////////////////////////////////////////////////////////////////////////////
GameScene::~GameScene()
{
	SDL_DestroyTexture(bookCover);
	for (auto & e : bookPages)
	{
		SDL_DestroyTexture(e);
	}	
	SDL_DestroyTexture(playerTexture);	
}
////////////////////////////////////////////////////////////////////////////////////////////
void
GameScene::Render(SDL_Renderer * renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, bookCover, NULL, NULL);
	SDL_Rect pageDest{ 20, 40, 600, 400 };
	// render one page at bottom so when turning the page it doesn't look just single page
	SDL_RenderCopy(renderer, bookPages[0], NULL, &pageDest);
	SDL_RenderCopy(renderer, bookPages[(int)pageFrame], NULL, &pageDest);
		
	if (titlePage->IsDirty())
	{
		titlePage->Compose(fonts["title"]);
		titlePage->RenderContent(renderer);
		titlePage->SetDirty(false);
	}
	if (page->IsDirty())
	{
		page->Compose(fonts["text"]);
		page->RenderContent(renderer);
		page->SetDirty(false);
	}
	if (endPage->IsDirty())
	{
		endPage->Compose(fonts["special"]);
		endPage->RenderContent(renderer);
		endPage->SetDirty(false);
	}
	if (pageIndex == 0)
	{
		SDL_Rect textArea = { 40, 180, 270, 600 };
		SDL_RenderCopy(renderer, titlePage->_pageTexture, &textSrc, &textArea);
	}
	else if (pageIndex == 1)
	{
		SDL_Rect textArea = { 40, 50, 270, 400 };
		SDL_RenderCopy(renderer, page->_pageTexture, &textSrc, &textArea);
		SDL_RenderCopy(renderer, playerTexture, NULL, &playerRect);
	}
	else if (pageIndex == 2)
	{
		SDL_Rect textArea = { 0, 200, 300, 600 };
		SDL_RenderCopy(renderer, endPage->_pageTexture, &textSrc, &textArea);
	}	
	
	if (fading)
	{
		// cover whole window with white color
		int width = 0, height = 0;
		SDL_GetWindowSize(Game::GetInstance()->GetWindow(), &width, &height);
		boxRGBA(renderer, 0, 0, width, height, 255, 255, 255, (int)alpha);
	}		
	
}
///////////////////////////////////////////////////////////////////////////////
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
	if (nextPageTurning)
	{
		pageFrame += seconds * 3.0f;
		textSrc.w -= seconds * 5000.0f;
		if (pageFrame > 5.0f)
		{
			textSrc.w = TEXT_MAX_WIDTH;
			nextPageTurning = false;
			pageFrame = 0.0f;
			pageIndex++;
		}			
	}

	if (previousPageTurning)
	{
		pageFrame -= seconds * 3.0f;
		textSrc.w -= seconds * 5000.0f;
		if (pageFrame < 1.0f)
		{
			textSrc.w = TEXT_MAX_WIDTH;
			previousPageTurning = false;
			pageFrame = 0.0f;
			pageIndex--;
		}		
	}

	// start playing background music if there is no music
	// this is a hack for SDL_mixer lib which can't handle multiple mp3's
	// use SDL_sound for better handling
	if (Mix_PlayingMusic() == 0)
		Mix_PlayMusic(Game::GetInstance()->music["background"], -1);
}
////////////////////////////////////////////////////////////////////////////////
void
GameScene::OnEvent(SDL_Event & ev)
{
	if (ev.type == SDL_KEYDOWN)
	{
		if (ev.key.keysym.sym == SDLK_SPACE)
		{
			if (Mix_PlayMusic(Game::GetInstance()->music["page"], 1) == -1) 
			{
				printf("Mix_PlayMusic: %s\n", Mix_GetError());				
			}
			nextPageTurning = true;
		}
		if (ev.key.keysym.sym == SDLK_LCTRL)
		{
			if (Mix_PlayMusic(Game::GetInstance()->music["page"], 1) == -1) 
			{
				printf("Mix_PlayMusic: %s\n", Mix_GetError());
			}
			pageFrame = 6.0f;
			previousPageTurning = true;
		}
	}
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

