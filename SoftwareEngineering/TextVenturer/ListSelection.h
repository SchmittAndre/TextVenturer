#pragma once

#include "GUIBase.h"

class ListSelection : public GUIBase
{
private:
    UINT left;
    UINT top;
    UINT width;
    UINT count;

    size_t selectionIndex;
    UINT scroll;
    std::vector<std::string> items;

    bool changed;

    bool enabled;
    bool selectionLocked;

    NotifyEvent onSelect;
    NotifyEvent onChange;                          

    void notifyChanges();

public:
    ListSelection(TextDisplay* textDisplay, UINT left, UINT top, UINT width, UINT count);
    
    void add(std::string text);
    void delAll();

    bool isSelected();

    std::string getSelectedString();
    void setSelectedString(std::string item);

    void setSelectionIndex(size_t index);
    int getSelectionIndex();

    void pressKey(byte key);

    void enable();
    void disable();
    void unlockSelection();

    bool selectionIsLocked();
    bool isEnabled();

    void update();

    void addOnSelect(void* self, EventFuncNotify func);
    void delOnSelect(void* self, EventFuncNotify func);

    void addOnChange(void* self, EventFuncNotify func);
    void delOnChange(void* self, EventFuncNotify func);

};