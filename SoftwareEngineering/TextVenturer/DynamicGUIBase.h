#pragma once

#include "GUIBase.h"

class DynamicGUIBase abstract : public GUIBase
{
private:
    ivec2 pos;

    bool changed;
    bool visible;

protected:
    virtual void clearDisplay() = 0;

    void notifyChanges();
    bool hasChanged();

public:              
    DynamicGUIBase(TextDisplay* textDisplay, ivec2 pos);

    void setPos(ivec2 pos);
    ivec2 getPos() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void update(float deltaTime);
};