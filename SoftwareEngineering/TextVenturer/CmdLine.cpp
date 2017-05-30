#include "stdafx.h"

#include "Adventure.h"
#include "LineInput.h"
#include "TextBox.h"

#include "CmdLine.h"

CmdLine::CmdLine(Controler & controler)
    : GameDisplayer(controler)
{  
    adventure = NULL;
}

CmdLine::~CmdLine()
{         
    delete adventure;
}

void CmdLine::setAdventure(Adventure & adventure)
{
    if (this->adventure)
        delete this->adventure;
    this->adventure = &adventure;
}

void CmdLine::notifyLoad()
{
    GameDisplayer::notifyLoad();

    assert(adventure);

    lineInput = new LineInputAdventure(
        getTextDisplay(),
        ivec2(1, getTextDisplay().getHeight() - 2),
        getTextDisplay().getWidth() - 2,
        *adventure);
    lineInput->enable();
    
    textBox = new ScrollingTextBox(
        getTextDisplay(),
        ivec2(1, 1),
        getTextDisplay().getWidth() - 2,
        getTextDisplay().getHeight() - 4);

    adventure->start(*this);
}

void CmdLine::notifyUnload()
{
    delete adventure;
    delete lineInput;
    delete textBox;
}

void CmdLine::write(std::string msg)
{
    textBox->writeToBuffer(msg + "$reset()");
}

void CmdLine::update(float deltaTime)
{
    lineInput->update(deltaTime);      
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
