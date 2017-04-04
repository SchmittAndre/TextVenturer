#pragma once

class TextDisplay;

class GUIBase abstract
{
private:
    TextDisplay* textDisplay;

public:
    GUIBase(TextDisplay* textDisplay);

    TextDisplay* getTextDisplay();
};