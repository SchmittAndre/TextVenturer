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
    getCurrentDisplayer()->notifyLoad();
}

Controler::~Controler()
{
    for (GameDisplayer* disp : displayer)
    {
        disp->notifyUnload();
        delete disp;
    }
}

TextDisplay * Controler::getTextDisplay()
{
    return textDisplay;
}

Game * Controler::getGame()
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
}

void Controler::changeDisplayer(DisplayerType type)
{
    textDisplay->clear();
    getCurrentDisplayer()->notifyUnload();
    currentDisplayer = type;
    getCurrentDisplayer()->notifyLoad();
}

GameDisplayer * Controler::getCurrentDisplayer()
{
    return displayer[currentDisplayer];
}
