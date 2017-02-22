#pragma once

#include "AdventureObject.h"

class CustomAdventureAction;

class Item : public AdventureObject
{
private:
    CommandArray* carryCommands;

    CustomAdventureAction* onTake;
    CustomAdventureAction* onPlace;

public:        
    Item();
    ~Item();

    void runOnTake();
    void runOnPlace();

    void setOnTake(CustomAdventureAction* onTake);
    void setOnPlace(CustomAdventureAction* onPlace);

    CommandArray* getCarryCommands();
};