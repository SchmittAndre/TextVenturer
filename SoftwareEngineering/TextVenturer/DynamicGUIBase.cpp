#include "stdafx.h"

#include "DynamicGUIBase.h"

void DynamicGUIBase::notifyChanges()
{
    changed = true;
}

bool DynamicGUIBase::hasChanged()
{
    return changed;
}

DynamicGUIBase::DynamicGUIBase(TextDisplay & textDisplay, ivec2 pos)
    : GUIBase(textDisplay)
{
    this->pos = pos;
    visible = true;
    changed = true;    
}

void DynamicGUIBase::setPos(ivec2 pos)
{
    if (getPos() == pos)
        return;
    clearDisplay();
    this->pos = pos;
    notifyChanges();
}

ivec2 DynamicGUIBase::getPos() const
{
    return pos;
}

void DynamicGUIBase::setVisible(bool visible)
{
    if (this->visible == visible)
        return;
    this->visible = visible;
    notifyChanges();
}

bool DynamicGUIBase::isVisible() const
{
    return visible;
}

void DynamicGUIBase::update(float deltaTime)
{
    changed = false;
}
