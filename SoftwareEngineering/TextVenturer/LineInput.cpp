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
    if (!changed)
        for (auto func : onChange)
            func.func(func.self, this);
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

LineInput::LineInput(TextDisplay * textDisplay, uvec2 pos, UINT width)
    : GUIBase(textDisplay, pos)
{
    this->width = width;
    enabled = false;
    notifyChanges();
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

        getTextDisplay()->clearLine(getPos().y, getPos().x, width);
        getTextDisplay()->write(getPos().x, getPos().y, '>');
        getTextDisplay()->write(getPos().x + 2, getPos().y, input.substr(inputScroll, width - 2));
        getTextDisplay()->setCursorPos(getPos().x + 2 + inputPos - inputScroll, getPos().y);

        changed = false;
    }
}

void LineInput::pressChar(byte c)
{
    if (!isEnabled())
        return;

    input = input.substr(0, inputPos) + (char)c + input.substr(inputPos);
    setInputPos(inputPos + 1);
    getTextDisplay()->resetCursorTime();
}

void LineInput::pressKey(byte key)
{   
    if (!isEnabled())
        return;

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

void LineInput::addOnChange(void* self, EventFuncNotify func)
{
    onChange.insert({ self, func });
}

void LineInput::delOnChange(void* self, EventFuncNotify func)
{
    onChange.erase({ self, func });
}

bool LineInput::isEnabled()
{
    return enabled;
}

void LineInput::enable()
{
    enabled = true;
    getTextDisplay()->setCursorVisible(true);
}

void LineInput::disable()
{
    enabled = false;
    getTextDisplay()->setCursorVisible(false);
}

LineInputAdventure::LineInputAdventure(TextDisplay * textDisplay, uvec2 pos, UINT width, Adventure* adventure)
    : LineInput(textDisplay, pos, width)
{
    this->adventure = adventure;
    msgSaved = false;
    historyIndex = 0;
}

void LineInputAdventure::pressKey(byte key)
{
    if (!isEnabled())
        return;
    
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
