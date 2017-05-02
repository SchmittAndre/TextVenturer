#pragma once

class TextDisplay;

class GUIBase abstract
{
private:
    TextDisplay* textDisplay;
    uvec2 pos;

public:
    GUIBase(TextDisplay* textDisplay, uvec2 pos);
    virtual ~GUIBase();

    TextDisplay* getTextDisplay() const;

    uvec2 getPos() const;
    virtual void setPos(uvec2 pos);
};