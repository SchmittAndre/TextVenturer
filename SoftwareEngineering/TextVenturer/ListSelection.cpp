#include "stdafx.h"

#include "TextDisplay.h"
#include "DisplayChar.h"

#include "ListSelection.h"

void ListSelection::notifyChanges()
{
    if (!changed)
        for (auto e : onChange)
            e.func(e.self, this);
    changed = true;
}

ListSelection::ListSelection(TextDisplay * textDisplay, UINT left, UINT top, UINT width, UINT count)
    : GUIBase(textDisplay)
{
    this->left = left;
    this->top = top;
    this->width = width;
    this->count = count;
    enabled = false;
    selectionLocked = false;
    setSelectionIndex(std::string::npos);
}

void ListSelection::add(std::string text)
{
    items.push_back(text);
    if (selectionIndex == std::string::npos)
        selectionIndex = 0;
    notifyChanges();
}

void ListSelection::delAll()
{
    items.clear();
    setSelectionIndex(std::string::npos);
}

bool ListSelection::isSelected()
{
    return selectionIndex != std::string::npos;
}

std::string ListSelection::getSelectedString()
{
    return items[selectionIndex];
}

void ListSelection::setSelectedString(std::string item)
{
    auto pos = std::find(items.cbegin(), items.cend(), item);
    if (pos != items.cend())
        setSelectionIndex(pos - items.cbegin());
}

void ListSelection::setSelectionIndex(size_t index)
{
    selectionIndex = index;
    notifyChanges();
}

int ListSelection::getSelectionIndex()
{
    return selectionIndex;
}

void ListSelection::pressKey(byte key)
{
    if (selectionIndex == -1)
        return;
    switch (key)
    {
    case VK_UP:
        if (enabled && !selectionLocked && selectionIndex > 0)
        {
            selectionIndex--;
            notifyChanges();
        }
        break;
    case VK_DOWN:
        if (enabled && !selectionLocked && selectionIndex < items.size() - 1)
        {
            selectionIndex++;
            notifyChanges();
        }
        break;
    case VK_RETURN:
        if (enabled)
        {
            for (auto e : onSelect)
                e.func(e.self, this);
            selectionLocked = true;
            notifyChanges();
        }
        break;
    }
}

void ListSelection::enable()
{
    enabled = true;
    notifyChanges();
}

void ListSelection::disable()
{
    enabled = false;
    selectionIndex = std::string::npos;
    notifyChanges();
}

void ListSelection::unlockSelection()
{
    selectionLocked = false;
    notifyChanges();
}

bool ListSelection::selectionIsLocked()
{
    return selectionLocked;
}

bool ListSelection::isEnabled()
{
    return enabled;
}

void ListSelection::update()
{
    if (changed)
    {
        if (isSelected())
        {
            if (selectionIndex < scroll)
                scroll = selectionIndex;
            else if (selectionIndex > count + scroll - 1)
                scroll = selectionIndex - count + 1;
        }
        else
            scroll = 0;

        for (UINT i = 0; i < count; i++)
        {
            int y = top + 1 + i * 2;
            
            getTextDisplay()->clearLine(y, left, width);

            if (i + scroll < items.size())
            {
                std::string line = items[i + scroll];
                Color c;
                if (!enabled || selectionLocked && i + scroll != selectionIndex)
                    c = Color(0.7f, 0.7f, 0.7f);
                else if (!selectionLocked)
                    c = Color(1.0f, 1.0f, 1.0f);
                if (line.size() > width - 4)
                    line = line.substr(0, width - 7) + "...";
                getTextDisplay()->write(left + 2, y, line, c);
                if (enabled && i + scroll == selectionIndex)
                {
                    getTextDisplay()->write(left, y, "[", c);
                    getTextDisplay()->write(left + width - 1, y, "]", c);
                }              
            }
        }

        if (scroll > 0)
            getTextDisplay()->write(left + width / 2 - 1, top, "/\\");
        else
            getTextDisplay()->clearLine(top, left + width / 2 - 1, 2);
                                                                 
        if (scroll + count < items.size())
            getTextDisplay()->write(left + width / 2 - 1, top + count * 2, "\\/");
        else
            getTextDisplay()->clearLine(top + count * 2, left + width / 2 - 1, 2);

        changed = false;
    }
}

void ListSelection::addOnSelect(void * self, EventFuncNotify func)
{
    onSelect.insert({ self, func });
}

void ListSelection::delOnSelect(void * self, EventFuncNotify func)
{
    onSelect.erase({ self, func });
}

void ListSelection::addOnChange(void * self, EventFuncNotify func)
{
    onChange.insert({ self, func });
}

void ListSelection::delOnChange(void * self, EventFuncNotify func)
{
    onChange.erase({ self, func });
}

