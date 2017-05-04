#include "stdafx.h"

#include "TextDisplay.h"
#include "Adventure.h"
#include "GameDisplayer.h"
#include "MainMenu.h"
#include "OptionMenu.h"
#include "CmdLine.h"
#include "Game.h"
#include "AdventureSelection.h"

#include "Controler.h"

Controler::Controler(Game* game, TextDisplay* textDisplay)
{
    this->game = game;
    this->textDisplay = textDisplay;
    
    displayer[dtMainMenu] = new MainMenu(this);
    displayer[dtAdventureSelection] = new AdventureSelection(this);
    displayer[dtOptionMenu] = new OptionMenu(this);
    displayer[dtAdventure] = new CmdLine(this);

    currentDisplayer = dtMainMenu;
    nextDisplayer = dtMainMenu;
    getCurrentDisplayer()->notifyLoad();
}

Controler::~Controler()
{
    getCurrentDisplayer()->notifyUnload();
    for (GameDisplayer* disp : displayer)
        delete disp;
}

TextDisplay * Controler::getTextDisplay() const
{
    return textDisplay;
}

Game * Controler::getGame() const
{
    return game;
}

void Controler::pressChar(byte c)
{
    getCurrentDisplayer()->pressChar(c);
}

void Controler::pressKey(byte key)
{
    getCurrentDisplayer()->pressKey(key);
}             

void Controler::update(float deltaTime)
{
    getCurrentDisplayer()->update(deltaTime);

    if (nextDisplayer != currentDisplayer)
    {
        textDisplay->clear();
        getCurrentDisplayer()->notifyUnload();
        currentDisplayer = nextDisplayer;
        getCurrentDisplayer()->notifyLoad();
    }
}

void Controler::changeDisplayer(DisplayerType type)
{
    nextDisplayer = type;    
    // TODO: epic change effects like making everything explode and such, toggle and customizable in options
}

GameDisplayer * Controler::getCurrentDisplayer() const
{
    return displayer[currentDisplayer];
}

CmdLine * Controler::getCmdLine() const
{
    return static_cast<CmdLine*>(displayer[dtAdventure]);
}
