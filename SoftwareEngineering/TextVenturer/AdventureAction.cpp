#include "stdafx.h"      
#include "TextDisplay.h"
#include "Controler.h"
#include "Command.h"
#include "Adventure.h"
#include "Player.h"
#include "Room.h"
#include "Location.h"
#include "Inventory.h"
#include "ItemCombiner.h"

#include "AdventureAction.h"

AdventureAction::AdventureAction(Adventure * adventure)
:   BaseAction(adventure->getControler())
{
    this->adventure = adventure;
}

Adventure * AdventureAction::getAdventure() const
{
    return adventure;
}

Player * AdventureAction::getPlayer() const
{
    return adventure->getPlayer();
}

Inventory * AdventureAction::getPlayerInv() const
{
    return getPlayer()->getInventory();
}

Room * AdventureAction::currentRoom() const
{
    return getPlayer()->currentRoom();
}

Location * AdventureAction::currentLocation() const
{
    return getPlayer()->currentLocation();
}

ItemCombiner * AdventureAction::getItemCombiner() const
{
    return adventure->getItemCombiner();
}
