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

    std::mutex bufferLock;

    bool instant;

    virtual void step() = 0;

public:
    TextBox(TextDisplay & textDisplay, ivec2 pos, UINT width, UINT height);

    void writeToBuffer(std::string msg);
    virtual void clear();

    UINT getWidth() const;
    UINT getHeight() const;

    bool getInstant();
    void setInstant(bool instant);

    TextDisplay::State & getState();

    void update(float deltaTime);
};

class ScrollingTextBox : public TextBox
{
private:
    bool newLine;

protected:
    void step();

public:
    ScrollingTextBox(TextDisplay & textDisplay, ivec2 pos, UINT width, UINT height);

    void clear();
};

class LimitedTextBox : public TextBox
{
private:
    UINT currentLine;

protected:
    void step();

public:
    LimitedTextBox(TextDisplay & textDisplay, ivec2 pos, UINT width, UINT height);

    void clear();
};