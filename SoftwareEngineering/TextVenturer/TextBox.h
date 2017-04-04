#pragma once

#include "GUIBase.h"
#include "TextDisplay.h"

class TextBox : public GUIBase
{
private:
    UINT left;
    UINT top;
    UINT width;
    UINT height;

    TextDisplay::State state;
    int writepos;
    bool newLine;
    std::queue<std::string> textbuffer;

public:
    TextBox(TextDisplay* textDisplay, UINT left, UINT top, UINT width, UINT height);

    void writeToBuffer(std::string msg);

    void update(float deltaTime);
};