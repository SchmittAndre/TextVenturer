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

    std::mutex reloadLock;
    std::mutex listUpdateLock;

    friend void onErrorListSelect(void * self, void * sender);
    friend void onErrorListChange(void * self, void * sender);

    void showReloading();
    void showNoError();
    void showDescription();

    void reload();

    void updateList();

public:                          
    AdventureErrorLog(Controler & controler);

    void setAdventure(Adventure & adventure);

    void notifyLoad();
    void notifyUnload();

    void pressKey(byte key);

    void update(float deltaTime);
};