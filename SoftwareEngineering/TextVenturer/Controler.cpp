#include "stdafx.h"
#include "TextDisplay.h"

#include "Controler.h"

void Controler::updateInput()
{
    for (int x = cursorMin; x < cursorMax; x++)
        textDisplay->write(x, textDisplay->getCursorPos().y, ' ');
    textDisplay->write(cursorMin, textDisplay->getCursorPos().y, input);
}

Controler::Controler(TextDisplay* textDisplay)
{
    this->textDisplay = textDisplay;
    textDisplay->setCursorVisible(true);
    textDisplay->setCursorPos(ivec2(3, textDisplay->getHeight() - 2));
    cursorMin = 3;
    cursorMax = textDisplay->getWidth() - 2;
}

void Controler::pressChar(byte c)
{
    switch (c)
    {
    case VK_BACK:
        if (textDisplay->getCursorPos().x > cursorMin)
        {
            ivec2 p = textDisplay->getCursorPos();
            p.x--;
            textDisplay->setCursorPos(p);
            textDisplay->resetCursorTime();
            input.resize(input.length() - 1);
            updateInput();
        }
        break;
    case VK_RETURN:
    {
        command(input);

        input = "";
        updateInput();
        textDisplay->setCursorPos(ivec2(cursorMin, textDisplay->getCursorPos().y));

        break;
    }
    case 127: // CTRL-BACK
    {
        ivec2 p = textDisplay->getCursorPos();
        p.x--;
        while (textDisplay->getChar(p) == ' ' && p.x >= cursorMin)
        {
            pressChar(VK_BACK);
            p.x--;
        }
        while (textDisplay->getChar(p) != ' ' && p.x >= cursorMin)
        {
            pressChar(VK_BACK);
            p.x--;
        }
        break;
    }                     
    default:
        if (textDisplay->getCursorPos().x < cursorMax)
        {
            ivec2 p = textDisplay->getCursorPos();
            p.x++;
            textDisplay->setCursorPos(p);
            textDisplay->resetCursorTime();
            input += c;
            updateInput();
        }
    }                   
}

void Controler::update(float deltaTime)
{

}

void Controler::command(string msg)
{
    ErrorDialog("Message Entered", msg);
}
