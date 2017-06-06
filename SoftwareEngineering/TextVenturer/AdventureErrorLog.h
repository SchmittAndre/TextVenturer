#pragma once

#include "GameDisplayer.h"

class Adventure;
class ListSelection;

class AdventureErrorLog : public GameDisplayer
{
private:
    Adventure * adventure;

    ListSelection * errorList;

    friend void onErrorListSelect(void * self, void * sender);
    friend void onErrorListChange(void * self, void * sender);

public:                          
    AdventureErrorLog(Controler & controler);

    void setAdventure(Adventure & adventure);

    void notifyLoad();
    void notifyUnload();

    void pressKey(byte key);

    void update(float deltaTime);
};