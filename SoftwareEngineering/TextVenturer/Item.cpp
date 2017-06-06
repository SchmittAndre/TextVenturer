#include "stdafx.h"

#include "Adventure.h"
#include "CommandSystem.h"
#include "CustomAdventureAction.h"

#include "Item.h"
               
AdventureObject::Type Item::getType() const
{
    return otItem;
}

Item::Item()
    : onTake(NULL)
    , onPlace(NULL)
{
}

Item::Item(FileStream & stream, AdventureLoadHelp & help)
    : AdventureObject(stream, help)
    , carryCommands(stream, help)
    , onPlace(CustomAdventureAction::loadConditional(stream, help.adventure))
    , onTake(CustomAdventureAction::loadConditional(stream, help.adventure))
{
    help.commandArrays.push_back(carryCommands);
}

Item::~Item()
{
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

CommandArray & Item::getCarryCommands()
{
    return carryCommands;
}

void Item::save(FileStream & stream, AdventureSaveHelp & help) const
{
    AdventureObject::save(stream, help);
    carryCommands.save(stream, help);
    help.commandArrays[&carryCommands] = static_cast<UINT>(help.commandArrays.size());
    CustomAdventureAction::saveConditional(stream, onPlace);
    CustomAdventureAction::saveConditional(stream, onTake);
}
