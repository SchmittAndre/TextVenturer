#include "stdafx.h"

#include "Adventure.h"
#include "LineInput.h"
#include "TextBox.h"

#include "CmdLine.h"

CmdLine::CmdLine(Controler * controler)
    : GameDisplayer(controler)
{  
    adventure = NULL;
    lineInput = NULL;
    textBox = NULL;
}

CmdLine::~CmdLine()
{            
    delete lineInput;
    delete textBox;
}

void CmdLine::setAdventure(Adventure * adventure)
{
    this->adventure = adventure;
}

void CmdLine::notifySwitch()
{    
    delete lineInput;
    lineInput = new LineInputAdventure(
        getTextDisplay(),
        getTextDisplay()->getHeight() - 2,
        1,
        getTextDisplay()->getWidth() - 2,
        adventure);
    
    delete textBox;
    textBox = new TextBox(
        getTextDisplay(),
        1,
        1,
        getTextDisplay()->getWidth() - 2,
        getTextDisplay()->getHeight() - 4);
}

void CmdLine::write(std::string msg)
{
    textBox->writeToBuffer(msg + "$reset()");
}

void CmdLine::update(float deltaTime)
{
    lineInput->update();      
    adventure->update();      
    textBox->update(deltaTime);
}

void CmdLine::pressChar(byte c)
{
    lineInput->pressChar(c);
}

void CmdLine::pressKey(byte key)
{
    lineInput->pressKey(key);
}
