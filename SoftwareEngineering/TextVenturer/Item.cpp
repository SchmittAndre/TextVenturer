#include "stdafx.h"

#include "CommandSystem.h"
#include "CustomAdventureAction.h"

#include "Item.h"
               
AdventureObject::Type Item::getType()
{
    return otItem;
}

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

CustomAdventureAction* Item::getOnTake()
{
    return onTake;
}

CustomAdventureAction* Item::getOnPlace()
{
    return onPlace;
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

void Item::save(FileStream & stream, idlist<AdventureObject*>& objectIDs, idlist<CommandArray*>& commandArrayIDs)
{
    AdventureObject::save(stream, objectIDs, commandArrayIDs);
    carryCommands->save(stream);
    saveAdventureAction(stream, onPlace);
    saveAdventureAction(stream, onTake);
}
