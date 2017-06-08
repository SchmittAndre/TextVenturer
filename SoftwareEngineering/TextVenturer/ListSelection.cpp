#include "stdafx.h"

#include "TextDisplay.h"
#include "DisplayChar.h"

#include "ListSelection.h"

void ListSelection::notifySelectionChanged()
{
    if (!hasChanged())
        for (auto e : onChange)
            e.func(e.self, this);
    notifyChanges();
}

void ListSelection::clearDisplay()
{   
    getTextDisplay().clearLine(getPos().y, getPos().x + width / 2 - 1, 2);
    getTextDisplay().clearLine(getPos().y + count * 2, getPos().x + width / 2 - 1, 2);
    for (UINT i = 0; i < count; i++)
        getTextDisplay().clearLine(getPos().y + 1 + i * 2, getPos().x, width);
}

ListSelection::ListSelection(TextDisplay & textDisplay, ivec2 pos, UINT width, UINT count)
    : DynamicGUIBase(textDisplay, pos)
{
    this->width = width;
    this->count = count;
    enabled = false;
    selectionLocked = false;
    setIndex(std::string::npos);
}

void ListSelection::add(std::string text, void* data)
{
    items.push_back({ text, data });
    if (selectionIndex == std::string::npos)
        selectionIndex = 0;
    notifySelectionChanged();
}

void ListSelection::delAll()
{
    items.clear();
    setIndex(std::string::npos);
}

bool ListSelection::isSelected() const
{
    return selectionIndex != std::string::npos;
}

std::string ListSelection::getSelectedString() const
{
    return items[selectionIndex].text;
}

void ListSelection::setSelectedByData(void * data)
{
    for (size_t i = 0; i < items.size(); i++)
        if (items[i].data == data)
            setIndex(i);
}

void ListSelection::setIndex(size_t index)
{
    if (index != std::string::npos)
        index = min(index, items.size() - 1);
    selectionIndex = index;
    notifySelectionChanged();
}

void ListSelection::incIndex(size_t amount)
{
    if (selectionIndex + amount >= items.size())
        setIndex(items.size() - 1);
    else
        setIndex(selectionIndex + amount);
}

void ListSelection::decIndex(size_t amount)
{
    if (selectionIndex < amount)
        setIndex(0);
    else
        setIndex(selectionIndex - amount);
}                 

size_t ListSelection::getIndex() const
{
    return selectionIndex;
}

void * ListSelection::getData(size_t index) const
{
    return items[index].data;
}

void * ListSelection::getSelectedData() const
{
    return items[getIndex()].data;
}

void ListSelection::pressKey(byte key)
{
    if (selectionIndex == std::string::npos)
        return;
    switch (key)
    {
    case VK_UP:
        if (enabled && !selectionLocked)
            decIndex(1);
        break;
    case VK_DOWN:
        if (enabled && !selectionLocked)
            incIndex(1);
        break;
    case VK_PRIOR:
        if (enabled && !selectionLocked)
            decIndex(count);
        break;
    case VK_NEXT:
        if (enabled && !selectionLocked)
            incIndex(count);
        break;
    case VK_RETURN:
        if (enabled && !selectionLocked)
        {
            selectionLocked = true;
            notifyChanges();
            for (auto e : onSelect)
                e.func(e.self, this);
        }
        break;
    }
}

void ListSelection::enable()
{
    enabled = true;
    if (items.size() > 0)
        selectionIndex = 0;
    notifySelectionChanged();
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

bool ListSelection::isEnabled() const
{
    return enabled;
}

UINT ListSelection::getWidth() const
{
    return width;
}

void ListSelection::setWidth(UINT width)
{
    if (this->width == width)
        return;
    clearDisplay();
    this->width = width;
    notifyChanges();
}

UINT ListSelection::getCount() const
{
    return count;
}

void ListSelection::setCount(UINT count)
{
    if (this->count == count)
        return;
    clearDisplay();
    this->count = count;
    notifyChanges();
}

void ListSelection::update(float deltaTime)
{
    if (hasChanged())
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
            int y = getPos().y + 1 + i * 2;
            
            getTextDisplay().clearLine(y, getPos().x, width);

            if (i + scroll < items.size())
            {
                std::string line = items[i + scroll].text;
                TextDisplay::State state;
                if (!enabled || selectionLocked && i + scroll != selectionIndex)
                    state.color = Color(0.7f, 0.7f, 0.7f);
                else if (!selectionLocked)
                    state.color = Color(1.0f, 1.0f, 1.0f);
                if (line.size() > width - 4)
                    line = line.substr(0, width - 7) + "...";
                getTextDisplay().writeAll(getPos().x + 2, y, line, state);
                if (enabled && i + scroll == selectionIndex)
                {
                    getTextDisplay().writeAll(getPos().x, y, "[", state);
                    getTextDisplay().writeAll(getPos().x + width - 1, y, "]", state);
                }              
            }
        }

        if (scroll > 0)
            getTextDisplay().write(getPos().x + width / 2 - 1, getPos().y, "/\\");
        else
            getTextDisplay().clearLine(getPos().y, getPos().x + width / 2 - 1, 2);
                                                                 
        if (scroll + count < items.size())
            getTextDisplay().write(getPos().x + width / 2 - 1, getPos().y + count * 2, "\\/");
        else
            getTextDisplay().clearLine(getPos().y + count * 2, getPos().x + width / 2 - 1, 2);

        DynamicGUIBase::update(deltaTime);
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

