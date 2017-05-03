#pragma once

class TextDisplay;

class GUIBase abstract
{
private:
    TextDisplay* textDisplay;

public:
    GUIBase(TextDisplay* textDisplay);
    virtual ~GUIBase();

    TextDisplay* getTextDisplay() const;

    virtual void update(float deltaTime);
};