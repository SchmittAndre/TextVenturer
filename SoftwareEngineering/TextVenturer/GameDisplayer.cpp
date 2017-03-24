#include "stdafx.h"

#include "TextDisplay.h"

#include "GameDisplayer.h"

TextDisplay * GameDisplayer::getTextDisplay()
{
    return textDisplay;
}

GameDisplayer::GameDisplayer(TextDisplay * textDisplay)
{
    this->textDisplay = textDisplay;
}

GameDisplayer::~GameDisplayer()
{
}
