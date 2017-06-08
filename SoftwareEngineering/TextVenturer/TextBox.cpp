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

            if (lineLength == width)
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
    state.reset();
    writepos = pos.x;
    while (!textbuffer.empty())
        textbuffer.pop();
    for (UINT line = pos.y; line < pos.y + height; line++)
        getTextDisplay().clearLine(line, pos.x, width);
}

UINT TextBox::getWidth() const
{
    return width;
}

UINT TextBox::getHeight() const
{
    return height;
}

bool TextBox::getInstant()
{
    return instant;
}

void TextBox::setInstant(bool instant)
{
    this->instant = instant;
}

TextDisplay::State & TextBox::getState()
{
    return state;
}

void TextBox::update(float deltaTime)
{
    if (!textbuffer.empty())
    {
        if (!instant)
            state.time = max(state.time - deltaTime, -1); // never more than 1 second behind what should happen
        
        while (!textbuffer.empty() && (state.time <= 0 || instant))
            step();
    }
}

ivec2 TextBox::getPos() const
{
    return pos;
}

TextBox::TextBox(TextDisplay & textDisplay, ivec2 pos, UINT width, UINT height)
    : GUIBase(textDisplay)
    , pos(pos)
    , width(width)
    , height(height)
    , instant(false)
{
    clear();
}

void ScrollingTextBox::step()
{
    if (newLine)
    {
        getTextDisplay().move(ivec2(getPos().x, getPos().y + 1), uvec2(getWidth(), getHeight() - 1), getPos());
        getTextDisplay().clearLine(getPos().y + getHeight() - 1, getPos().x, getWidth());
        newLine = false;
    }
    getTextDisplay().writeStep(writepos, getPos().y + getHeight() - 1, textbuffer.front(), state);
    if (textbuffer.front().empty())
    {
        // next line
        writepos = getPos().x;
        textbuffer.pop();
        newLine = true;
    }
}

ScrollingTextBox::ScrollingTextBox(TextDisplay & textDisplay, ivec2 pos, UINT width, UINT height)
    : TextBox(textDisplay, pos, width, height)
    , newLine(false)
{
}

void ScrollingTextBox::clear()
{
    TextBox::clear();
    newLine = false;
}

void LimitedTextBox::step()
{
    if (currentLine >= getHeight())
    {
        // just discard everything that doesn't fit
        textbuffer.pop();
    }
    else
    {
        getTextDisplay().writeStep(writepos, getPos().y + currentLine, textbuffer.front(), state);
        if (textbuffer.front().empty())
        {
            // next line
            writepos = getPos().x;
            textbuffer.pop();
            currentLine++;
        }
    }
}

LimitedTextBox::LimitedTextBox(TextDisplay & textDisplay, ivec2 pos, UINT width, UINT height)
    : TextBox(textDisplay, pos, width, height)
    , currentLine(0)
{
}

void LimitedTextBox::clear()
{
    TextBox::clear();
    currentLine = 0;
}
