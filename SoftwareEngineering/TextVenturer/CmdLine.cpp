#include "stdafx.h"

#include "Adventure.h"
#include "CmdLine.h"

void CmdLine::updateInput()
{
    if (inputPos < inputScroll)
        inputScroll = inputPos;
    else if (inputPos >= inputScroll + getTextDisplay()->getWidth() - 5)
        inputScroll = inputPos - getTextDisplay()->getWidth() + 5;

    getTextDisplay()->clearLine(getTextDisplay()->getHeight() - 2);
    getTextDisplay()->write(1, getTextDisplay()->getHeight() - 2, '>');
    getTextDisplay()->write(3, getTextDisplay()->getHeight() - 2, input.substr(inputScroll, getTextDisplay()->getWidth() - 4));
    getTextDisplay()->setCursorPos(3 + inputPos - inputScroll, getTextDisplay()->getHeight() - 2);
}

void CmdLine::writeToBuffer(std::string msg)
{
    std::string line;
    size_t lineLength = 0;
    size_t lineStart = 0;
    size_t lastSpace = std::string::npos;
    for (size_t p = 0; p < msg.size(); p++)
    {
        std::smatch matches;
        if (msg[p] != '$' || p == msg.size() - 1 || msg[p + 1] == '$' ||
            !std::regex_search(msg.cbegin() + p, msg.cend(), matches, std::regex("\\$[^$ ]+?\\([^ ]*?\\)"), std::regex_constants::match_continuous))
        {
            // not a $ or a $ at end of line or $$ or does not fit the correct syntax
            // write normally

            if (msg[p] == ' ')
                lastSpace = p - lineStart;

            if (lineLength == getTextDisplay()->getWidth() - 2)
            {
                if (lastSpace == std::string::npos || lastSpace == p - lineStart)
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
                lastSpace = std::string::npos;
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

    textbuffer.push(line);
}

CmdLine::CmdLine(Controler * controler)
    : GameDisplayer(controler)
{  
    adventure = NULL;
}

CmdLine::~CmdLine()
{                
}

void CmdLine::setAdventure(Adventure * adventure)
{
    this->adventure = adventure;
}

void CmdLine::notifySwitch()
{    
    getTextDisplay()->setCursorVisible(true);
    getTextDisplay()->setCursorPos(ivec2(3, getTextDisplay()->getHeight() - 2));
    newLine = false;
    writepos = 1;
    msgSaved = false;
    updateInput();
}

void CmdLine::write(std::string msg)
{
    writeToBuffer(msg + "$reset()");
}

void CmdLine::update(float deltaTime)
{
    if (!textbuffer.empty())
    {
        state.time = max(state.time - deltaTime, -1); // never more than 1 second behind what should happen
        while (!textbuffer.empty() && state.time <= 0)
        {
            if (newLine)
            {
                getTextDisplay()->move(ivec2(1, 2), uvec2(getTextDisplay()->getWidth() - 2, getTextDisplay()->getHeight() - 5), ivec2(1, 1));
                getTextDisplay()->clearLine(getTextDisplay()->getHeight() - 4, 1, getTextDisplay()->getWidth() - 2);
                newLine = false;
            }
            getTextDisplay()->writeStep(writepos, getTextDisplay()->getHeight() - 4, textbuffer.front(), state);
            if (textbuffer.front().empty())
            {
                // next line
                writepos = 1;
                textbuffer.pop();
                newLine = true;
            }
        }
    }

    adventure->update();
}

void CmdLine::pressChar(byte c)
{
    input = input.substr(0, inputPos) + (char)c + input.substr(inputPos);
    inputPos++;
    updateInput();

    getTextDisplay()->resetCursorTime();
}

void CmdLine::pressKey(byte key)
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

                getTextDisplay()->resetCursorTime();
            }
        }
        break;
    case VK_RETURN:
    {
        if (input == "")
            break;

        adventure->sendCommand(input);

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

        getTextDisplay()->resetCursorTime();

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
                inputPos = (UINT)input.size();
                updateInput();
                getTextDisplay()->resetCursorTime();
                historyIndex++;
                msgSaved = true;
            }
        }
        else if (historyIndex < inputHistory.size() - 1)
        {
            historyIndex++;
            input = inputHistory[historyIndex];
            inputPos = (UINT)input.size();
            updateInput();

            getTextDisplay()->resetCursorTime();
        }
        break;
    }
    case VK_DOWN:
    {
        if (historyIndex > 1)
        {
            historyIndex--;
            input = inputHistory[historyIndex];
            inputPos = (UINT)input.size();
            updateInput();
            getTextDisplay()->resetCursorTime();
        }
        else if (historyIndex == 1)
        {
            historyIndex--;
            input = inputHistory[0];
            inputHistory.erase(inputHistory.begin());
            inputPos = (UINT)input.size();
            updateInput();
            getTextDisplay()->resetCursorTime();
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

                getTextDisplay()->resetCursorTime();
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

                getTextDisplay()->resetCursorTime();
            }
        }
        break;
    }
    case VK_DELETE:
    {
        if (inputPos < input.size())
        {
            static bool ignore = false;
            if (!ignore && GetAsyncKeyState(VK_CONTROL) & 0x8000)
            {
                ignore = true;
                while (inputPos < input.size() && input[inputPos] != ' ')
                    pressKey(VK_DELETE);
                while (inputPos < input.size() && input[inputPos] == ' ')
                    pressKey(VK_DELETE);
                ignore = false;
            }
            else
            {
                input = input.substr(0, inputPos) + input.substr(inputPos + 1);
                updateInput();

                getTextDisplay()->resetCursorTime();
            }
        }
        break;
    }
    }
}
