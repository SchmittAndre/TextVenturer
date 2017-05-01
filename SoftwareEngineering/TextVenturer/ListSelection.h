#pragma once

#include "GUIBase.h"

class ListSelection : public GUIBase
{
private:
    struct Entry
    {
        std::string text;
        void* data;
    };

    UINT width;
    UINT count;

    size_t selectionIndex;
    size_t scroll;
    std::vector<Entry> items;

    bool changed;

    bool enabled;
    bool selectionLocked;

    NotifyEvent onSelect;
    NotifyEvent onChange;                          

    void notifyChanges();
    void notifySelectionChanged();

    void clearDisplay();

public:
    ListSelection(TextDisplay* textDisplay, uvec2 pos, UINT width, UINT count);
    
    void add(std::string text, void* data = NULL);
    void delAll();

    bool isSelected() const;

    std::string getSelectedString() const;
    void setSelectedByData(void* data);

    void setIndex(size_t index);
    size_t getIndex() const;

    void* getData(size_t index) const;
    void* getSelectedData() const;

    void pressKey(byte key);

    void enable();
    void disable();
    void unlockSelection();  
    bool selectionIsLocked();
    bool isEnabled();

    UINT getWidth();
    void setWidth(UINT width);
    
    UINT getCount();
    void setCount(UINT count);

    void setPos(uvec2 pos);

    void update();

    void addOnSelect(void* self, EventFuncNotify func);
    void delOnSelect(void* self, EventFuncNotify func);

    void addOnChange(void* self, EventFuncNotify func);
    void delOnChange(void* self, EventFuncNotify func);

};