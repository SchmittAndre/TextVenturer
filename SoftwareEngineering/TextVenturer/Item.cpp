#include "stdafx.h"

#include "CommandSystem.h"
#include "CustomAdventureAction.h"

#include "Item.h"
               
AdventureObject::Type Item::getType() const
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

CustomAdventureAction* Item::getOnTake() const
{
    return onTake;
}

CustomAdventureAction* Item::getOnPlace() const
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

CommandArray * Item::getCarryCommands() const
{
    return carryCommands;
}

void Item::save(FileStream & stream, idlist<AdventureObject*>& objectIDs, idlist<CommandArray*>& commandArrayIDs) const
{
    AdventureObject::save(stream, objectIDs, commandArrayIDs);
    carryCommands->save(stream);
    commandArrayIDs[carryCommands] = static_cast<UINT>(commandArrayIDs.size());
    saveAdventureAction(stream, onPlace);
    saveAdventureAction(stream, onTake);
}

void Item::load(FileStream & stream, Adventure * adventure, std::vector<AdventureObject*>& objectList, std::vector<CommandArray*>& commandArrayList)
{
    AdventureObject::load(stream, adventure, objectList, commandArrayList);
    carryCommands->load(stream, adventure);
    commandArrayList.push_back(carryCommands);
    loadAdventureAction(stream, adventure, onPlace);
    loadAdventureAction(stream, adventure, onTake);
}
