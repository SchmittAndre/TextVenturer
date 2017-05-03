#include "stdafx.h"

#include "TextDisplay.h"

#include "GUIBase.h"

GUIBase::GUIBase(TextDisplay * textDisplay)
{
    this->textDisplay = textDisplay;
}

GUIBase::~GUIBase()
{
}

TextDisplay * GUIBase::getTextDisplay() const
{
    return textDisplay;
}

void GUIBase::update(float deltaTime)
{
    // nothing
}
