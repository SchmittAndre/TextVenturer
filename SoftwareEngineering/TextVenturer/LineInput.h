#pragma once

#include "GUIBase.h"

class LineInput : public GUIBase
{
private:
    // Visual Position 
    UINT width;

    // Current String
    std::string input;
    bool changed;

    // Writing Position + Side-Scroll
    UINT inputPos;
    UINT inputScroll;

    NotifyEvent onChange;

    bool enabled;

protected:
    void notifyChanges();

    void setInputPos(UINT inputPos);
    UINT getInputPos();

public:
    LineInput(TextDisplay* textDisplay, uvec2 pos, UINT width);
    ~LineInput();

    void update();

    std::string getInput();
    void setInput(const std::string input);
    
    void pressChar(byte c);
    virtual void pressKey(byte key);

    void addOnChange(void* self, EventFuncNotify func);
    void delOnChange(void* self, EventFuncNotify func);

    bool isEnabled();

    void enable();
    void disable();
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
    LineInputAdventure(TextDisplay* textDisplay, uvec2 pos, UINT width, Adventure* adventure);

    void pressKey(byte key);

};