#pragma once

#include "AdventureObject.h"

class Item : public AdventureObject
{
private:
    CommandArray* carryCommands;
public:        
    Item();
    ~Item();

    CommandArray* getCarryCommands();
};