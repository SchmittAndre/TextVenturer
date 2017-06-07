#pragma once

#include "DynamicGUIBase.h"

class LineInput : public DynamicGUIBase
{
private:
    // Visual Position 
    UINT width;

    // Current String
    std::string input;

    // Writing Position + Side-Scroll
    UINT inputPos;
    UINT inputScroll;

    NotifyEvent onChange;

    bool enabled;
    bool inputChanged;

protected:                  
    void clearDisplay();

    void setInputPos(UINT inputPos);
    UINT getInputPos() const;

public:
    LineInput(TextDisplay & textDisplay, ivec2 pos, UINT width);
    ~LineInput();

    void update(float deltaTime);

    std::string getInput() const;
    void setInput(const std::string input);
    
    void pressChar(byte c);
    virtual void pressKey(byte key);

    void addOnChange(void* self, EventFuncNotify func);
    void delOnChange(void* self, EventFuncNotify func);

    bool isEnabled() const;

    void enable();
    void disable();
};

class Adventure;

class LineInputAdventure : public LineInput
{
private:
    Adventure & adventure;

    std::vector<std::string> history;
    bool msgSaved;
    UINT historyIndex;

public:
    LineInputAdventure(TextDisplay & textDisplay, ivec2 pos, UINT width, Adventure & adventure);

    void pressKey(byte key);

};