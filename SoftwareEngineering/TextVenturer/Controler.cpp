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

void Controler::write(string msg)
{
    writeToBuffer(msg + "$reset()");
}

void Controler::writeToBuffer(string msg)
{
    string line;
    size_t lineLength = 0;
    size_t lineStart = 0;
    size_t lastSpace = string::npos;
    for (size_t p = 0; p < msg.size(); p++)
    {
        smatch matches;
        if (msg[p] != '$' || p == msg.size() - 1 || msg[p + 1] == '$' || 
            !regex_search(msg.cbegin() + p, msg.cend(), matches, regex("^\\$[^$ ]+?\\([^ ]*?\\)")))
        {
            // not a $ or a $ at end of line or $$ or does not fit the correct syntax
            // write normally
            
            if (msg[p] == ' ')
                lastSpace = p - lineStart;

            if (lineLength == textDisplay->getWidth() - 2)
            {             
                if (lastSpace == string::npos || lastSpace == p - lineStart)
                {
                    // no space in last line, cut the huge word
                    textbuffer.push(line);
                    line = "";
                }
                else
                {        
                    // cut it at the last space and skip all following spaces
                    textbuffer.push(line.substr(0, line.find_last_not_of(' ', lastSpace) + 1));
                    line = line.substr(lastSpace + 1);
                }

                while (msg[p] == ' ')
                {
                    p++;
                    lastSpace = p - lineStart;
                }

                lineLength = 0;
                lineStart = p - line.size();
                p -= line.size() + 1;
                line = "";
                lastSpace = string::npos;
            }
            else
            {
                line += msg[p];
                lineLength++;
            }

            if (msg[p] == '$' && msg[p + 1] == '$') 
            {
                line += '$'; // add the second $ but skip for lineLength
                p++;
            }
        }    
        else
        {
            // fits the correct syntax, add to line but don't increase lineLength
            line += msg.substr(p, matches[0].length());
            p += matches[0].length() - 1;
        }           
    }

    if (lineLength > 0)
        textbuffer.push(line); 
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
