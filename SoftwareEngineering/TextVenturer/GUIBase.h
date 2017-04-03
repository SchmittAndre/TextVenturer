#pragma once

class TextDisplay;

class GUIBase
{
private:
    TextDisplay* textDisplay;

public:
    GUIBase(TextDisplay* textDisplay);

    TextDisplay* getTextDisplay();
};