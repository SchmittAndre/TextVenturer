#include "stdafx.h"

#include "TextDisplay.h"
#include "Controler.h"

#include "GameDisplayer.h"

Controler * GameDisplayer::getControler() const
{
    return controler;
}

void GameDisplayer::notifyLoad()
{
    loaded = true;
}

void GameDisplayer::notifyUnload()
{
    loaded = false;
}

bool GameDisplayer::active() const
{
    return loaded;
}

TextDisplay * GameDisplayer::getTextDisplay()
{
    return controler->getTextDisplay();
}

GameDisplayer::GameDisplayer(Controler* controler)
{
    this->controler = controler;
    loaded = false;
}

GameDisplayer::~GameDisplayer()
{
}
