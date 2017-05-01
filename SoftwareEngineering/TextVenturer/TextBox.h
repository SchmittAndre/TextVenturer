#pragma once

#include "GUIBase.h"
#include "TextDisplay.h"

class TextBox abstract : public GUIBase
{
protected:
    UINT width;
    UINT height;

    TextDisplay::State state;
    int writepos;
    std::queue<std::string> textbuffer;

public:
    TextBox(TextDisplay* textDisplay, uvec2 pos, UINT width, UINT height);

    void writeToBuffer(std::string msg);
    virtual void clear();

    virtual void update(float deltaTime) = 0;
};

class ScrollingTextBox : public TextBox
{
private:
    bool newLine;
   
public:
    ScrollingTextBox(TextDisplay* textDisplay, uvec2 pos, UINT width, UINT height);

    void clear();
    void update(float deltaTime);
};

class LimitedTextBox : public TextBox
{
private:
    UINT currentLine;

public:
    LimitedTextBox(TextDisplay* textDisplay, uvec2 pos, UINT width, UINT height);

    void clear();
    void update(float deltaTime);
};