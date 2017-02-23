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

    CustomAdventureAction* getOnTake();
    CustomAdventureAction* getOnPlace();

    void setOnTake(CustomAdventureAction* onTake);
    void setOnPlace(CustomAdventureAction* onPlace);

    CommandArray* getCarryCommands();
};