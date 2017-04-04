#include "stdafx.h"

#include "TextDisplay.h"
#include "Adventure.h"

#include "LineInput.h"

std::string LineInput::getInput()
{
    return input;
}

void LineInput::setInput(const std::string input)
{
    if (this->input == input)
        return;
    this->input = input;
    notifyChanges();
}

void LineInput::notifyChanges()
{
    changed = true;
}

void LineInput::setInputPos(UINT inputPos)
{
    inputPos = min(max(inputPos, 0), (UINT)input.size());
    if (this->inputPos == inputPos)
        return;
    this->inputPos = inputPos;
    notifyChanges();
}

UINT LineInput::getInputPos()
{
    return inputPos;
}

LineInput::LineInput(TextDisplay * textDisplay, UINT line, UINT left, UINT width)
    : GUIBase(textDisplay)
{
    this->line = line;
    this->left = left;
    this->width = width;
    changed = true;
}

LineInput::~LineInput()
{
}

void LineInput::update()
{
    if (changed)
    {            
        if (inputPos < inputScroll)
            inputScroll = inputPos;
        else if (inputPos >= inputScroll + width - 3)
            inputScroll = inputPos - width + 3;

        getTextDisplay()->clearLine(line, left, width);         
        getTextDisplay()->write(left, line, '>');
        getTextDisplay()->write(left + 2, line, input.substr(inputScroll, width - 2));
        getTextDisplay()->setCursorPos(left + 2 + inputPos - inputScroll, line);

        changed = false;
    }
}

void LineInput::pressChar(byte c)
{
    input = input.substr(0, inputPos) + (char)c + input.substr(inputPos);
    setInputPos(inputPos + 1);
    getTextDisplay()->resetCursorTime();
}

void LineInput::pressKey(byte key)
{
    switch (key)
    {
    case VK_BACK:
    {
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
                setInputPos(inputPos - 1);
                input = input.substr(0, inputPos) + input.substr(inputPos + 1);
                getTextDisplay()->resetCursorTime();
            }
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
                setInputPos(inputPos - 1);
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
                setInputPos(inputPos + 1);         
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
                setInput(input.substr(0, inputPos) + input.substr(inputPos + 1));
                getTextDisplay()->resetCursorTime();
            }
        }
        break;
    }
    }
}

LineInputAdventure::LineInputAdventure(TextDisplay * textDisplay, UINT line, UINT left, UINT width, Adventure* adventure)
    : LineInput(textDisplay, line, left, width)
{
    this->adventure = adventure;
    msgSaved = false;
    historyIndex = 0;
    getTextDisplay()->setCursorVisible(true);
}

void LineInputAdventure::pressKey(byte key)
{
    switch (key)
    {
    case VK_RETURN:
    {
        if (getInput() == "")
            break;

        adventure->sendCommand(getInput());

        if (msgSaved)
        {
            history.erase(history.begin());
            msgSaved = false;
        }
        history.insert(history.begin(), getInput());
        historyIndex = 0;

        setInput("");
        setInputPos(0);
        notifyChanges();
        getTextDisplay()->resetCursorTime();

        break;
    }
    case VK_UP:
    {
        if (historyIndex == 0 && !msgSaved)
        {
            if (history.size() > 0)
            {
                history.insert(history.begin(), getInput());
                setInput(history[1]);
                setInputPos((UINT)getInput().size());
                getTextDisplay()->resetCursorTime();
                historyIndex++;
                msgSaved = true;
            }
        }
        else if (historyIndex < history.size() - 1)
        {
            historyIndex++;
            setInput(history[historyIndex]);
            setInputPos((UINT)getInput().size());
            getTextDisplay()->resetCursorTime();
        }
        break;
    }
    case VK_DOWN:
    {
        if (historyIndex > 1)
        {
            historyIndex--;
            setInput(history[historyIndex]);
            setInputPos((UINT)getInput().size());
            getTextDisplay()->resetCursorTime();
        }
        else if (historyIndex == 1)
        {
            historyIndex--;
            setInput(history[0]);
            history.erase(history.begin());
            setInputPos((UINT)getInput().size());
            getTextDisplay()->resetCursorTime();
            msgSaved = false;
        }
        break;
    }
    default:
        LineInput::pressKey(key);
    }
}
