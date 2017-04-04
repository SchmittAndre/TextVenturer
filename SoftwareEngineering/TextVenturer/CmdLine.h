#pragma once

#include "GameDisplayer.h"

class Adventure;
class LineInput;
class TextBox;

class CmdLine : public GameDisplayer
{
private:
    Adventure* adventure;   

    TextBox* textBox;
    LineInput* lineInput;

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