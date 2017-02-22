#include "stdafx.h"

#include "CommandSystem.h"
#include "CustomAdventureAction.h"

#include "Item.h"
               
Item::Item()
{
    carryCommands = new CommandArray();
    onTake = NULL;
    onPlace = NULL;
}

Item::~Item()
{
    delete carryCommands;
    delete onTake;
    delete onPlace;
}

void Item::runOnTake()
{
    if (onTake)
        onTake->run();
}

void Item::runOnPlace()
{
    if (onPlace)
        onPlace->run();
}

void Item::setOnTake(CustomAdventureAction * onTake)
{
    this->onTake = onTake;
}

void Item::setOnPlace(CustomAdventureAction * onPlace)
{
    this->onPlace = onPlace;
}

CommandArray * Item::getCarryCommands()
{
    return carryCommands;
}
