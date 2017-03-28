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
    
    mainMenu = new MainMenu(this);
    adventureSelection = new AdventureSelection(this);
    optionMenu = new OptionMenu(this);
    cmdLine = new CmdLine(this);

    adventure = new Adventure();
    cmdLine->setAdventure(adventure);
    adventure->loadFromFile(L"data\\adventure\\the quest for the bow.txvs");
    adventure->start(cmdLine);
    changeDisplayer(dtAdventure);
}

Controler::~Controler()
{
    delete adventure;

    delete mainMenu;
    delete adventureSelection;
    delete optionMenu;
    delete cmdLine;
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
    textDisplay->clear();
    switch (type)
    {
    case dtMainMenu:
        currentDisplayer = mainMenu;
        break;
    case dtAdventureSelection:
        currentDisplayer = adventureSelection;
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
