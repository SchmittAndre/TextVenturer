#include "stdafx.h"

#include "Adventure.h"
#include "LineInput.h"
#include "TextBox.h"

#include "CmdLine.h"

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
    lineInput = NULL;
}

CmdLine::~CmdLine()
{            
    delete lineInput;
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
    delete lineInput;
    lineInput = new LineInputAdventure(
        getTextDisplay(),
        getTextDisplay()->getHeight() - 2,
        1,
        getTextDisplay()->getWidth() - 2,
        adventure);
}

void CmdLine::write(std::string msg)
{
    writeToBuffer(msg + "$reset()");
}

void CmdLine::update(float deltaTime)
{
    lineInput->update();

    adventure->update();

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
}

void CmdLine::pressChar(byte c)
{
    lineInput->pressChar(c);
}

void CmdLine::pressKey(byte key)
{
    lineInput->pressKey(key);
}
