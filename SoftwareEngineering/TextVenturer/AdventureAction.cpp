#include "stdafx.h"     

#include "AdventureObject.h"
#include "Adventure.h"
#include "Player.h"
#include "Inventory.h"
#include "Room.h"
#include "RoomConnection.h"
#include "Item.h"
#include "ItemCombiner.h"
#include "DefaultAdventureAction.h"
#include "CommandSystem.h"
#include "CustomAdventureAction.h"

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

bool AdventureAction::changeRoom(RoomConnection * connection, bool showDescription) const
{
    Room* fromRoom = currentRoom();
    Room* toRoom = connection->getOtherRoom(fromRoom);

    if (!changeLocation(NULL, false))
        return false;

    // all action have no action or action does not override
    if ((!fromRoom->getOnLeave() || !fromRoom->getOnLeave()->overrides()) &&
        (!connection->getOnUse() || !connection->getOnUse()->overrides()) &&
        (!toRoom->getOnEnter() || !toRoom->getOnEnter()->overrides()))
    {
        getPlayer()->inform(connection);
        getPlayer()->inform(toRoom);
        getPlayer()->gotoRoom(toRoom);
        write("You went through " + connection->getName(getPlayer()) + " and entered " + toRoom->getName(getPlayer()) + ".");
        if (showDescription)
            write(toRoom->getDescription());
    }

    if (!toRoom)
    {
        ErrorDialog("Room Connection " + connection->getNameOnly() + " has no connection to Room " + fromRoom->getNameOnly() + ".");
        return false;
    }
    
    if (fromRoom->getOnLeave())
    {
        fromRoom->getOnLeave()->run();
        if (fromRoom->getOnLeave()->overrides())
            return false;
    }

    if (connection->getOnUse())
    {
        connection->getOnUse()->run();
        if (connection->getOnUse()->overrides())
            return false;
    }

    if (toRoom->getOnEnter())
    {
        toRoom->getOnEnter()->run();
        if (toRoom->getOnEnter()->overrides())
            return false;
    }

    return true;
}

bool AdventureAction::changeLocation(Location * location, bool showDescription) const
{
    Location* oldLocation = currentLocation();
    bool atLocationAlready = oldLocation == location;
   
    if (atLocationAlready ||
        (!oldLocation || !oldLocation->getOnLeave() || !oldLocation->getOnLeave()->overrides()) &&
        (!location || !location->getOnGoto() || !location->getOnGoto()->overrides()))
    {                       
        if (location)
        {
            if (!atLocationAlready)
            {
                getPlayer()->inform(location);
                getPlayer()->gotoLocation(location);
                write("You went to " + location->getName(getPlayer()) + ".");
            }
            if (showDescription)
            {
                write(location->getDescription());
            }
        }
    }

    if (atLocationAlready)
        return true;

    if (oldLocation && oldLocation->getOnLeave())
    {
        oldLocation->getOnLeave()->run();
        if (oldLocation->getOnLeave()->overrides())
            return false;
    }

    if (location && location->getOnGoto())
    {
        location->getOnGoto()->run();
        if (location->getOnGoto()->overrides())
            return false;
    }                
    return true;
}

void AdventureAction::combine(Item * item1, Item * item2, Item * result) const
{
    CustomAdventureAction* action = getItemCombiner()->getOnCombine(item1, item2);
    
    if (!action || !action->overrides())
    {
        getPlayerInv()->delItem(item1);
        getPlayerInv()->delItem(item2);
        getPlayerInv()->addItem(result);
        write("You combined the two and received " + result->getName(getPlayer()) + ".");
        getPlayer()->inform(result);
    }

    if (action)
        action->run();             
}

void AdventureAction::take(Location::PInventory * inventory, Item * item) const
{
    if (!item->getOnTake() || !item->getOnTake()->overrides())
    {
        inventory->delItem(item);
        getPlayerInv()->addItem(item);
        getPlayer()->inform(item);
        write("You took " + item->getName(getPlayer()) +
            " " + inventory->getPrepositionName(true) +
            " " + currentLocation()->getName(getPlayer()) + ".");
    }

    if (item->getOnTake())
        item->getOnTake()->run();
}

void AdventureAction::place(Location::PInventory * inventory, Item * item) const
{
    if (!item->getOnPlace() || !item->getOnPlace()->overrides())
    {
        getPlayerInv()->delItem(item);
        write("You placed " + item->getName(getPlayer()) + 
            " " + inventory->getPrepositionName() + 
            " " + currentLocation()->getName(getPlayer()) + ".");
    }

    if (item->getOnPlace())
        item->getOnPlace()->run();          
}

void AdventureAction::inspect(AdventureObject * object) const
{
    if (!object->getOnInspect() || !object->getOnInspect()->overrides())
    {
        getPlayer()->inform(object);
        write(object->getDescription());
        if (Location* location = dynamic_cast<Location*>(object))
        {
            if (location->filledInventoryCount() > 0)
            {
                std::string content;
                auto invs = location->getInventories();
                for (auto inv = invs.begin(); inv != invs.end(); inv++)
                {
                    if ((*inv)->isEmpty())
                        continue;
                    if (inv == invs.end() - 1 && content != "")
                        content += " and ";
                    content += (*inv)->formatContents(getPlayer()) +
                        " " + (*inv)->getPrepositionName() +
                        " " + location->getName(getPlayer());
                    if (invs.size() > 1 && inv < invs.end() - 2)
                        content += ", ";
                }
                std::string be = location->firstFilledInventory()->getItemCount() > 1 ||
                    location->firstFilledInventory()->getItems()[0]->isNamePlural() ? "are " : "is ";
                write("There " + be + content + ".");
            }
        }
        if (RoomConnection* connection = dynamic_cast<RoomConnection*>(object))
        {
            if (connection->isAccessible())
            {
                write(connection->getName(getPlayer(), true) + " leads to " + connection->getOtherRoom(currentRoom())->getName(getPlayer()) + ".");
            }
        }
    }
    
    if (object->getOnInspect())
        object->getOnInspect()->run();
}

