#pragma once

#include "GameDisplayer.h"
#include "TextDisplay.h"

class Adventure;
class LineInput;

class CmdLine : public GameDisplayer
{
private:
    Adventure* adventure;   

    // output
    TextDisplay::State state;
    int writepos;
    bool newLine;
    std::queue<std::string> textbuffer;

    LineInput* lineInput;

    void writeToBuffer(std::string msg);

public:
    CmdLine(Controler * controler);
    ~CmdLine();

    void setAdventure(Adventure* adventure);
    void notifySwitch();

    void write(std::string msg);

    void update(float deltaTime);

    void pressChar(byte c);
    void pressKey(byte key);

};