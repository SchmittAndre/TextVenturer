#pragma once

#include "GameDisplayer.h"
#include "TextDisplay.h"

class Adventure;

class CmdLine : public GameDisplayer
{
private:
    Adventure* adventure;

    TextDisplay::State state;
    int writepos;
    bool newLine;
    std::queue<std::string> textbuffer;

    std::string input;
    UINT inputPos;
    UINT inputScroll;

    std::vector<std::string> inputHistory;
    bool msgSaved;
    UINT historyIndex;

    void updateInput();
    void writeToBuffer(std::string msg);

public:
    CmdLine(TextDisplay* textDisplay);
    ~CmdLine();

    void setAdventure(Adventure* adventure);
    void notifySwitch();

    void write(std::string msg);

    void update(float deltaTime);

    void pressChar(byte c);
    void pressKey(byte key);

};