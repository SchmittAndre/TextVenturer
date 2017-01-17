#include "stdafx.h"
#include "Command.h"
#include "CommandSystem.h"
#include "TextDisplay.h"
#include "Game.h"
#include "DefaultAction.h"
#include "Adventure.h"
#include "Window.h" 

#include "Controler.h"

void Controler::updateInput()
{
    for (int x = cursorMin; x < cursorMax; x++)
        textDisplay->write(x, textDisplay->getCursorPos().y, ' ');
    textDisplay->write(cursorMin, textDisplay->getCursorPos().y, input);
}

Controler::Controler(TextDisplay* textDisplay, Game* game)
{
    defaultAction = new DefaultAction(this);
    commandSystem = new CommandSystem(this, defaultAction);
    this->textDisplay = textDisplay;
	this->game = game;
    textDisplay->setCursorVisible(true);
    textDisplay->setCursorPos(ivec2(3, textDisplay->getHeight() - 2));
    cursorMin = 3;
    cursorMax = textDisplay->getWidth() - 2;

    newLine = false;
    writepos = 1;

    textDisplay->write(1, textDisplay->getHeight() - 2, '>');
}

Controler::~Controler()
{
    delete adventure;
    delete defaultAction;
    delete commandSystem;
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
    if (!textbuffer.empty())
    {
        state.time = state.time - deltaTime;

        while (!textbuffer.empty() && state.time <= 0)
        {
            if (newLine)
            {
                textDisplay->move(ivec2(1, 2), uvec2(textDisplay->getWidth() - 2, textDisplay->getHeight() - 5), ivec2(1, 1));
                textDisplay->clearLine(textDisplay->getHeight() - 4, 1, textDisplay->getWidth() - 2);
                newLine = false;
            }
            size_t count = textDisplay->writeSingleChar(ivec2(writepos++, textDisplay->getHeight() - 4), textbuffer.front(), state);
            textbuffer.front() = textbuffer.front().substr(count);
            if (textbuffer.front().empty())
            {
                writepos = 1;
                textbuffer.pop();
                newLine = true;
            }
        }
    }
}

void Controler::writeLine(string msg, TextDisplay::State & state)
{
    // put a arbitrary char in front, so there is something in front of a possible command at the start of the string
    string text = "x" + msg; // , regex("^\\$[^$ ]+?\\([^ ]*?\\)"), "");
    
    // replace "[^$]$___(___)" with "[^$]" one after another, because of overlapping in the match
    {
        string old;
        do
        {
            old = text;
            text = regex_replace(old, regex("([^$])\\$[^$ ]+?\\([^ ]*?\\)"), "$1", regex_constants::format_first_only);
        } while (old != text);
    }

    // replace "$$" with "$"
    text = regex_replace(text, regex("\\$\\$"), "$");        

    // remove the arbitrary char at start of line again
    text = text.substr(1);

    size_t sizediff = msg.size() - text.size();

    if (text.size() > textDisplay->getWidth() - 2)
    {
        size_t spacePos = msg.find_last_of(' ', textDisplay->getWidth() - 2 + sizediff);
        if (spacePos == string::npos)
            spacePos = textDisplay->getWidth() - 2 + sizediff;
        textbuffer.push(msg.substr(0, spacePos));
        writeLine(msg.substr(msg.find_first_not_of(' ', spacePos)));
    }
    else
        textbuffer.push(msg + "$reset()");
}

void Controler::command(string msg)
{
    if (adventure && adventure->isInitialized())
    {
        adventure->sendCommand(msg);
    }
    else
    {
        commandSystem->sendCommand(msg);
    }
}

void Controler::DEBUG_startAdventure()
{
    adventure = new Adventure(this);
}
