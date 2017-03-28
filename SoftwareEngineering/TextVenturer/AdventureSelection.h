#pragma once

#include "GameDisplayer.h"

class Adventure;

class AdventureSelection : public GameDisplayer
{
private:
    std::vector<Adventure*> adventures;
    UINT selected;
    
    bool listChanged;

    void unloadAdventures();
    void loadAdventures();

public:
    AdventureSelection(Controler* controler);
    ~AdventureSelection();

    void notifySwitch();

    void update(float deltaTime);
};