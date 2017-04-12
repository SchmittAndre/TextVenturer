#pragma once

#include "GUIBase.h"

class LineInput : public GUIBase
{
private:
    // Visual Position 
    UINT line;
    UINT left;
    UINT width;

    // Current String
    std::string input;
    bool changedFlag;

    // Writing Position + Side-Scroll
    UINT inputPos;
    UINT inputScroll;

protected:
    void notifyChanges();

    void setInputPos(UINT inputPos);
    UINT getInputPos();

public:
    LineInput(TextDisplay* textDisplay, UINT line, UINT left, UINT width);
    virtual ~LineInput();

    void update();

    std::string getInput();
    void setInput(const std::string input);
    
    void pressChar(byte c);
    virtual void pressKey(byte key);

    bool changed();
};

class Adventure;

class LineInputAdventure : public LineInput
{
private:
    Adventure* adventure;

    std::vector<std::string> history;
    bool msgSaved;
    UINT historyIndex;

public:
    LineInputAdventure(TextDisplay* textDisplay, UINT line, UINT left, UINT width, Adventure* adventure);

    void pressKey(byte key);

};