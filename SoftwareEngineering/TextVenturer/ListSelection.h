#pragma once

#include "GUIBase.h"

class ListSelection : public GUIBase
{
private:
    UINT left;
    UINT top;
    UINT width;
    UINT count;

    int selectionIndex;
    UINT scroll;
    std::vector<std::string> items;

    bool changed;

public:
    ListSelection(TextDisplay* textDisplay, UINT left, UINT top, UINT width, UINT count);
    
    void add(std::string text);
    void delAll();

    void setSelectionIndex(int index);
    int getSelectionIndex();

    void pressKey(byte key);

    void update();

};