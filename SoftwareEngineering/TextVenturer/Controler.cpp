#include "stdafx.h"

#include "TextDisplay.h"
#include "Adventure.h"
#include "GameDisplayer.h"
#include "MainMenu.h"
#include "OptionMenu.h"
#include "CmdLine.h"

#include "Controler.h"

Controler::Controler(TextDisplay* textDisplay)
{
    this->textDisplay = textDisplay;
    
    mainMenu = new MainMenu(textDisplay);
    optionMenu = new OptionMenu(textDisplay);
    cmdLine = new CmdLine(textDisplay);

    mainMenu->notifySwitch();
    adventure = NULL;
}

Controler::~Controler()
{
    delete mainMenu;
    delete optionMenu;
    delete cmdLine;

    delete adventure;
}

void Controler::pressChar(byte c)
{
    //cmdLine->pressChar(c);
}

void Controler::pressKey(byte key)
{
    //cmdLine->pressKey(key);
    mainMenu->pressKey(key);
}             

void Controler::update(float deltaTime)
{
    //cmdLine->update(deltaTime);
    mainMenu->update(deltaTime);
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
