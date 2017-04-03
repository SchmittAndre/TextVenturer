#include "stdafx.h"

#include "TextDisplay.h"

#include "GUIBase.h"

GUIBase::GUIBase(TextDisplay * textDisplay)
{
    this->textDisplay = textDisplay;
}

TextDisplay * GUIBase::getTextDisplay()
{
    return textDisplay;
}
