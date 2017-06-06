#pragma once

#include "GameDisplayer.h"

class Adventure;
class ListSelection;
class TextBox;

class AdventureErrorLog : public GameDisplayer
{
private:
    Adventure * adventure;

    ListSelection * errorList;
    TextBox * infoBox;

    friend void onErrorListSelect(void * self, void * sender);
    friend void onErrorListChange(void * self, void * sender);

    void showDescription();

public:                          
    AdventureErrorLog(Controler & controler);

    void setAdventure(Adventure & adventure);

    void notifyLoad();
    void notifyUnload();

    void pressKey(byte key);

    void update(float deltaTime);
};