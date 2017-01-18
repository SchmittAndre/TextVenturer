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
    if (inputPos < inputScroll)
        inputScroll = inputPos;
    else if (inputPos >= inputScroll + textDisplay->getWidth() - 5)
        inputScroll = inputPos - textDisplay->getWidth() + 5;

    textDisplay->clearLine(textDisplay->getHeight() - 2);
    textDisplay->write(1, textDisplay->getHeight() - 2, '>');
    textDisplay->write(3, textDisplay->getHeight() - 2, input.substr(inputScroll, textDisplay->getWidth() - 4));
    textDisplay->setCursorPos(3 + inputPos - inputScroll,  textDisplay->getHeight() - 2);
}

Controler::Controler(TextDisplay* textDisplay, Game* game)
{
    defaultAction = new DefaultAction(this);
    commandSystem = new CommandSystem(this, defaultAction);
    this->textDisplay = textDisplay;
	this->game = game;
    textDisplay->setCursorVisible(true);
    textDisplay->setCursorPos(ivec2(3, textDisplay->getHeight() - 2));
    
    newLine = false;
    writepos = 1;
    msgSaved = false;

    //for (size_t i = 0; i < textDisplay->getHeight(); i++)
    //    writeLine("$delay(0)$shaking(0.1)||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");

    updateInput();
}

Controler::~Controler()
{
    delete adventure;
    delete defaultAction;
    delete commandSystem;
}

void Controler::pressChar(byte c)
{
    input = input.substr(0, inputPos) + (char)c + input.substr(inputPos);
    inputPos++;
    updateInput();   

    textDisplay->resetCursorTime();
}

void Controler::pressKey(byte key)
{
    switch (key)
    {
    case VK_BACK:
        if (inputPos > 0)
        {
            static bool ignore = false;
            if (!ignore && GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                ignore = true;
                while (inputPos > 0 && input[inputPos - 1] == ' ')
                    pressKey(VK_BACK);
                while (inputPos > 0 && input[inputPos - 1] != ' ')
                    pressKey(VK_BACK);
                ignore = false;
            }
            else
            {
                inputPos--;
                input = input.substr(0, inputPos) + input.substr(inputPos + 1);
                updateInput();

                textDisplay->resetCursorTime();
            }
        }
        break;
    case VK_RETURN:
    {
        if (input == "")
            break;

        command(input);
        if (msgSaved)
        {
            inputHistory.erase(inputHistory.begin());
            msgSaved = false;
        }
        inputHistory.insert(inputHistory.begin(), input);
        historyIndex = 0;

        input = "";
        inputPos = 0;
        inputScroll = 0;
        updateInput();

        textDisplay->resetCursorTime();

        break;
    }
    case VK_UP:
    {
        if (historyIndex == 0 && !msgSaved)
        {
            if (inputHistory.size() > 0)
            {
                inputHistory.insert(inputHistory.begin(), input);
                input = inputHistory[1];
                inputPos = input.size();
                updateInput();
                historyIndex++;
                msgSaved = true;
            }
        }
        else if (historyIndex < inputHistory.size() - 1)
        {
            historyIndex++;
            input = inputHistory[historyIndex];
            inputPos = input.size();
            updateInput();
        }
        break;
    }
    case VK_DOWN:
    {
        if (historyIndex > 1)
        {
            historyIndex--;
            input = inputHistory[historyIndex];
            inputPos = input.size();
            updateInput();
        }
        else if (historyIndex == 1)
        {
            historyIndex--;
            input = inputHistory[0];
            inputHistory.erase(inputHistory.begin());
            inputPos = input.size();
            updateInput();
            msgSaved = false;
        }
        break;
    }
    case VK_LEFT:
    {   
        if (inputPos > 0)
        {
            static bool ignore = false;
            if (!ignore && GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                ignore = true;
                while (inputPos > 0 && input[inputPos - 1] == ' ')
                    pressKey(VK_LEFT);
                while (inputPos > 0 && input[inputPos - 1] != ' ')
                    pressKey(VK_LEFT);
                ignore = false;
            }
            else
            {
                inputPos--;
                updateInput();

                textDisplay->resetCursorTime();
            }
        }
        break;
    }
    case VK_RIGHT:
    {
        if (inputPos < input.size())
        {
            static bool ignore = false;
            if (!ignore && GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                ignore = true;
                while (inputPos < input.size() && input[inputPos] != ' ')
                    pressKey(VK_RIGHT);
                while (inputPos < input.size() && input[inputPos] == ' ')
                    pressKey(VK_RIGHT);
                ignore = false;
            }
            else
            {
                inputPos++;
                updateInput();

                textDisplay->resetCursorTime();
            }
        }
        break;
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
            textDisplay->writeStep(writepos, textDisplay->getHeight() - 4, textbuffer.front(), state);
            if (textbuffer.front().empty())
            {
                // next line
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
