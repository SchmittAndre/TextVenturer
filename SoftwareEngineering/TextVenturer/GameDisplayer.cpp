#include "stdafx.h"

#include "TextDisplay.h"
#include "Controler.h"

#include "GameDisplayer.h"

Controler * GameDisplayer::getControler()
{
    return controler;
}

TextDisplay * GameDisplayer::getTextDisplay()
{
    return controler->getTextDisplay();
}

GameDisplayer::GameDisplayer(Controler* controler)
{
    this->controler = controler;
}

GameDisplayer::~GameDisplayer()
{
}
