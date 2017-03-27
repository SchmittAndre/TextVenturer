#include "stdafx.h"

#include "TextDisplay.h"
#include "Adventure.h"
#include "GameDisplayer.h"
#include "MainMenu.h"
#include "OptionMenu.h"
#include "CmdLine.h"
#include "Game.h"

#include "Controler.h"

Controler::Controler(Game* game, TextDisplay* textDisplay)
{
    this->game = game;
    this->textDisplay = textDisplay;
    
    mainMenu = new MainMenu(this);
    optionMenu = new OptionMenu(this);
    cmdLine = new CmdLine(this);

    adventure = NULL;

    changeDisplayer(dtMainMenu);
}

Controler::~Controler()
{
    delete mainMenu;
    delete optionMenu;
    delete cmdLine;

    delete adventure;
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
    currentDisplayer->pressChar(c);
}

void Controler::pressKey(byte key)
{
    currentDisplayer->pressKey(key);
}             

void Controler::update(float deltaTime)
{
    currentDisplayer->update(deltaTime);
}

void Controler::changeDisplayer(DisplayerType type)
{
    switch (type)
    {
    case dtMainMenu:
        currentDisplayer = mainMenu;
        break;
    case dtOptionMenu:
        currentDisplayer = optionMenu;
        break;
    case dtAdventure:
        currentDisplayer = cmdLine;
        break;
    }
    currentDisplayer->notifySwitch();
}

bool Controler::loadAdventure(std::string filename)
{
    adventure = new Adventure(cmdLine);
    //cmdLine->setAdventure(adventure);
    return adventure->loadFromFile(filename);
}

bool Controler::loadAdventureState(std::string filename)
{
    adventure = new Adventure(cmdLine);
    //cmdLine->setAdventure(adventure);
    return adventure->loadState(filename);
}

bool Controler::saveAdventureState(std::string filename)
{
    return adventure->saveState(filename);
}

void Controler::startAdventure()
{
    adventure->start();
}

void Controler::unloadAdventure()
{
    delete adventure;
    adventure = NULL;
    //cmdLine->setAdventure(NULL);
}
