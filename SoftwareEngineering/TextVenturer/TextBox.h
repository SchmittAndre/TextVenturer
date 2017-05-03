#pragma once

#include "GUIBase.h"
#include "TextDisplay.h"

class TextBox abstract : public GUIBase
{
private:
    UINT width;
    UINT height;   
    ivec2 pos;

protected:
    ivec2 getPos() const;

    TextDisplay::State state;
    int writepos;
    std::queue<std::string> textbuffer;

public:
    TextBox(TextDisplay* textDisplay, ivec2 pos, UINT width, UINT height);

    void writeToBuffer(std::string msg);
    virtual void clear();

    UINT getWidth() const;
    UINT getHeight() const;
};

class ScrollingTextBox : public TextBox
{
private:
    bool newLine;
   
public:
    ScrollingTextBox(TextDisplay* textDisplay, ivec2 pos, UINT width, UINT height);

    void clear();
    void update(float deltaTime);
};

class LimitedTextBox : public TextBox
{
private:
    UINT currentLine;

public:
    LimitedTextBox(TextDisplay* textDisplay, ivec2 pos, UINT width, UINT height);

    void clear();
    void update(float deltaTime);
};