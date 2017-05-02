#include "stdafx.h"

#include "TextDisplay.h"

#include "GUIBase.h"

GUIBase::GUIBase(TextDisplay * textDisplay, uvec2 pos)
{
    this->textDisplay = textDisplay;
    this->pos = pos;
}

GUIBase::~GUIBase()
{
}

TextDisplay * GUIBase::getTextDisplay() const
{
    return textDisplay;
}

uvec2 GUIBase::getPos() const
{
    return pos;
}

void GUIBase::setPos(uvec2 pos)
{
    this->pos = pos;
}
