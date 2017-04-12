#include "stdafx.h"

#include "TextDisplay.h"

#include "ListSelection.h"

ListSelection::ListSelection(TextDisplay * textDisplay, UINT left, UINT top, UINT width, UINT count)
    : GUIBase(textDisplay)
{
    this->left = left;
    this->top = top;
    this->width = width;
    this->count = count;
    changed = true;
}

void ListSelection::add(std::string text)
{
    items.push_back(text);
    changed = true;
}

void ListSelection::delAll()
{
    items.clear();
    changed = true;
}

void ListSelection::setSelectionIndex(int index)
{
    selectionIndex = index;
    changed = true;
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
        selectionIndex = max(0, selectionIndex - 1);
        changed = true;
        break;
    case VK_DOWN:
        selectionIndex = min((int)items.size() - 1, selectionIndex + 1);
        changed = true;
        break;
    }
}

void ListSelection::update()
{
    if (changed)
    {
        if (selectionIndex < scroll)
            scroll = selectionIndex;
        else if (selectionIndex > count + scroll - 1)
            scroll = selectionIndex - count + 1;

        for (UINT i = 0; i < count; i++)
        {
            int y = top + 1 + i * 2;
            
            getTextDisplay()->clearLine(y, left, width);

            if (i < items.size() - scroll)
            {
                std::string line = items[i + scroll];
                if (line.size() > width - 4)
                    line = line.substr(0, width - 7) + "...";
                getTextDisplay()->write(left + 2, y, line);
                if (i + scroll == selectionIndex)
                {
                    getTextDisplay()->write(left, y, "[");
                    getTextDisplay()->write(left + width - 1, y, "]");
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

