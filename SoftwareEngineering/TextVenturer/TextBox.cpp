#include "stdafx.h"

#include "TextBox.h"

void TextBox::writeToBuffer(std::string msg)
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

void TextBox::clear()
{
    while (!textbuffer.empty())
        textbuffer.pop();
    for (UINT line = top; line < top + height; line++)
        getTextDisplay()->clearLine(line, left, width);
}

TextBox::TextBox(TextDisplay* textDisplay, UINT left, UINT top, UINT width, UINT height)
    : GUIBase(textDisplay)
{
    this->left = left;
    this->top = top;
    this->width = width;
    this->height = height;
    writepos = 1;
}

ScrollingTextBox::ScrollingTextBox(TextDisplay * textDisplay, UINT left, UINT top, UINT width, UINT height)
    : TextBox(textDisplay, left, top, width, height)
{

}

void ScrollingTextBox::clear()
{
    TextBox::clear();
    newLine = false;
}

void ScrollingTextBox::update(float deltaTime)
{
    if (!textbuffer.empty())
    {
        state.time = max(state.time - deltaTime, -1); // never more than 1 second behind what should happen
        while (!textbuffer.empty() && state.time <= 0)
        {
            if (newLine)
            {
                getTextDisplay()->move(ivec2(left, top + 1), uvec2(width, height - 1), ivec2(left, top));
                getTextDisplay()->clearLine(top + height - 1, left, width);
                newLine = false;
            }
            getTextDisplay()->writeStep(writepos, top + height - 1, textbuffer.front(), state);
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

LimitedTextBox::LimitedTextBox(TextDisplay * textDisplay, UINT left, UINT top, UINT width, UINT height)
    : TextBox(textDisplay, left, top, width, height)
{
    currentLine = 0;
}

void LimitedTextBox::clear()
{
    TextBox::clear();
    currentLine = 0;
}

void LimitedTextBox::update(float deltaTime)
{
    if (!textbuffer.empty())
    {
        state.time = max(state.time - deltaTime, -1); // never more than 1 second behind what should happen
        while (!textbuffer.empty() && state.time <= 0)
        {
            if (currentLine >= height) 
            {
                // just discard everthing that doesn't fit
                textbuffer.pop();
            }
            else
            {
                getTextDisplay()->writeStep(writepos, top + currentLine, textbuffer.front(), state);
                if (textbuffer.front().empty())
                {
                    // next line
                    writepos = 1;
                    textbuffer.pop();
                    currentLine++;
                }
            }
        }
    }
}