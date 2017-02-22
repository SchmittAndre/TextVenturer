#include "stdafx.h"

#include "CommandSystem.h"

#include "Item.h"
               
Item::Item()
{
    carryCommands = new CommandArray();
}

Item::~Item()
{
    delete carryCommands;
}

CommandArray * Item::getCarryCommands()
{
    return carryCommands;
}
