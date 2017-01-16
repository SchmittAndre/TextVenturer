#include "stdafx.h"
#include "Command.h"     
#include "TextDisplay.h"
#include "Controler.h"
#include "Adventure.h"
#include "Player.h"
#include "Inventory.h"
#include "Item.h"
#include "Location.h"
#include "Room.h"
#include "Container.h"
#include "ItemCombiner.h"
#include "RoomConnection.h"

#include "DefaultAdventureAction.h"

void DefaultAdventureAction::run(const Command::Result & params) const
{
    // The input does not match with any available command
    write("I didn't quiet get that...");
}

void HelpAction::run(const Command::Result & params) const
{
    // Give the player some helpful advice               
    write("I'm sorry, but I can't help you at the moment!");
}

void LookAroundAction::run(const Command::Result & params) const
{
    write("You can see " + getAdventure()->getPlayer()->currentRoom()->formatLocations() + ".");
}

void ShowInventoryAction::run(const Command::Result & params) const
{
    // List the players inventory
    if (getAdventure()->getPlayer()->getInventory()->isEmpty())
    {
        write("You are not carrying anything.");
    }
    else
    {
        write("You are carrying " + getAdventure()->getPlayer()->getInventory()->formatContents() + ".");
    }
}

void InspectAction::run(const Command::Result & params) const
{
    // Inspect a location and show the description, also goes there
    if (Container* container = dynamic_cast<Container*>(getAdventure()->getPlayer()->currentLocation()))
    {
        if (container->isAccessible())
        {
            if (Item* item = container->getInventory()->findItem(params["location"]))
            {
                write(item->getDescription());
                return;
            }
        }
    }

    if (Room* room = getAdventure()->getPlayer()->currentRoom()->findRoom(params["location"]))
    {
        getAdventure()->getPlayer()->gotoRoom(room);
        write("You entered " + room->getName(true) + ".");
        write(room->getDescription());
    }
    else if (getAdventure()->getPlayer()->currentRoom()->getAliases()->has(params["location"]))
    {
        write(getAdventure()->getPlayer()->currentRoom()->getDescription());
    }
    else if (Location* location = getAdventure()->getPlayer()->currentRoom()->findLocation(params["location"]))
    {
        getAdventure()->getPlayer()->gotoLocation(location);
        write(location->getDescription());
        if (Container* container = dynamic_cast<Container*>(location))
        {
            if (container->isAccessible())
            {
                if (!container->getInventory()->isEmpty())
                {
                    string be = container->getInventory()->getItemCount() > 1 ? "are " : "is ";
                    write("There " + be + container->getInventory()->formatContents() + " in " + container->getName(true) + ".");
                }
            }
        }
        else if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        {
            if (connection->isAccessible())
            {
                write("It leads to " + connection->getOtherRoom(getAdventure()->getPlayer()->currentRoom())->getName() + ".");
            }
        }
    }
    else if (Item* item = getAdventure()->getPlayer()->getInventory()->findItem(params["location"]))
    {
        write(item->getDescription());
    }
    else
    {
        write("Where exactly is that, if I shall ask?");
    }
}

void PickupAction::run(const Command::Result & params) const
{
    // Pick up an item from a location, if it is accessible
    if (Location* location = getAdventure()->getPlayer()->currentRoom()->findLocation(params["item"]))
    {
        write("You can't pick up " + location->getName(true) + ".");
    }
    else if (getAdventure()->getPlayer()->currentRoom()->getAliases()->has(params["item"]))
    {
        write("Are you crazy? You can't pick up " + getAdventure()->getPlayer()->currentRoom()->getName(true) + ".");
    }
    else if (Room* room = getAdventure()->getPlayer()->currentRoom()->findRoom(params["item"]))
    {
        write("Are you insane? You can't pick up " + room->getName(true) + ".");
    }
    else if (Container* container = dynamic_cast<Container*>(getAdventure()->getPlayer()->currentLocation()))
    {
        if (container->isAccessible())
        {
            if (Item* item = container->getInventory()->findItem(params["item"]))
            {
                container->getInventory()->delItem(item);
                getAdventure()->getPlayer()->getInventory()->addItem(item);
                write("You picked up " + item->getName(true) + ".");
            }              
            else
            {
                write("There is no " + Alias(params["item"]).nameOnly() + " here.");
            }
        }
        else
        {
            write("There is no " + Alias(params["item"]).nameOnly() + " here.");
        }
    }
    else
    {
        write("There is no " + Alias(params["item"]).nameOnly() + " here.");
    }
}

void UseRoomConnectionAction::run(const Command::Result & params) const
{
    // Use a room connection to get to another room if it is accessible
    if (Location* location = getAdventure()->getPlayer()->currentRoom()->findLocation(params["door"]))
    {
        if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        {
            if (connection->isAccessible())
            {
                Room* room = connection->getOtherRoom(getAdventure()->getPlayer()->currentRoom());
                getAdventure()->getPlayer()->gotoRoom(room);
                write("You entered " + room->getName(true) + ".");
            }
            else
            {
                write(connection->getDescription());
            }
        }
        else
        {
            write("You can't go through " + location->getName(true) + "!");
        }
    }
    else
    {
        write("Where is that?");
    }
}

void GotoAction::run(const Command::Result & params) const
{
    // Go to a specific location in the current room
    if (Location* location = getAdventure()->getPlayer()->currentRoom()->findLocation(params["location"]))
    {
        if (getAdventure()->getPlayer()->currentLocation() == location)
        {
            write("You are at " + location->getName(true) + " already.");
        }
        else
        {
            getAdventure()->getPlayer()->gotoLocation(location);
            write("You went to " + location->getName(true) + ".");
        }
    }
    else if (getAdventure()->getPlayer()->currentRoom()->getAliases()->has(params["location"]))
    {
        write("You are in " + getAdventure()->getPlayer()->currentRoom()->getName(true) + " already.");
    }
    else if (Room* room = getAdventure()->getPlayer()->currentRoom()->findRoom(params["location"]))
    {
        getAdventure()->getPlayer()->gotoRoom(room);
        write("You entered " + room->getName(true) + ".");
    }
    else
    {
        write("Here is no " + Alias(params["location"]).nameOnly() + ".");
    }
}

void CombineItemsAction::run(const Command::Result & params) const
{
    // Combine two items from the players inventory if possible
    Item* item1 = getAdventure()->getPlayer()->getInventory()->findItem(params["item1"]);
    Item* item2 = getAdventure()->getPlayer()->getInventory()->findItem(params["item2"]);
    if (item1 && item2)
    {
        if (item1 == item2)
        {
            write("You can't combine " + item1->getName(true) + " with itself!");
        }
        else if (Item* result = getAdventure()->getItemCombiner()->getResult(item1, item2))
        {
            getAdventure()->getPlayer()->getInventory()->delItem(item1);
            getAdventure()->getPlayer()->getInventory()->delItem(item2);
            getAdventure()->getPlayer()->getInventory()->addItem(result);
            write("You combined the two and received " + result->getName() + ".");
        }
        else
        {
            write("You can't combine " + item1->getName(true) + " with " + item2->getName(true) + ".");
        }
    }        
    else if (item2)
    {
        write("You have no " + Alias(params["item1"]).nameOnly() + ".");
    }
    else if (item1)
    {
        write("You have no " + Alias(params["item2"]).nameOnly() + ".");
    }
    else
    {
        write("You have neither " + Alias(params["item1"]).nameOnly() + " nor " + Alias(params["item2"]).nameOnly() + "!");
    }
}

void UseItemAction::run(const Command::Result & params) const
{
    // Use an item with a location
    if (Item* item = getAdventure()->getPlayer()->getInventory()->findItem(params["item"]))
    {
        if (Location* location = getAdventure()->getPlayer()->currentRoom()->findLocation(params["location"]))
        {
            if (AdventureAction* action = location->getItemAction(item))
            {
                getAdventure()->getPlayer()->gotoLocation(location);
                write("You used " + item->getName(true) + " with " + location->getName(true) + ".");
                action->run(params);
                getAdventure()->getPlayer()->getInventory()->delItem(item);
            }
            else
            {
                write("You can't do anything with " + item->getName(true) + " and " + location->getName(true) + ".");
            }
        }
        else
        {
            write("Where is that?");
        }
    }    
    else
    {
        write("You have no " + Alias(params["item"]).nameOnly() + ".");
    }
}

void LockLocationAction::run(const Command::Result & params) const
{
    // Lock a container or room connection
    Location* location = getAdventure()->getPlayer()->currentRoom()->findLocation(params["location"]);
    if (Container* container = dynamic_cast<Container*>(location))
    {
        container->lock();
        write(container->getDescription());
    }
    else if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
    {
        connection->lock();
        write(connection->getDescription());
    }
    else
    {
        ErrorDialog("This shouldn't have happened", "Location can't be locked");
    }
}

void UnlockLocationAction::run(const Command::Result & params) const
{
    // Unlock a container or room connection
    Location* location = getAdventure()->getPlayer()->currentRoom()->findLocation(params["location"]);
    if (Container* container = dynamic_cast<Container*>(location))
    {
        container->unlock();
        write(container->getDescription());
        if (!container->getInventory()->isEmpty())
        {
            string be = container->getInventory()->getItemCount() > 1 ? "are " : "is ";
            write("There " + be + container->getInventory()->formatContents() + " in " + container->getName(true) + ".");
        }
    }
    else if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
    {
        connection->unlock();
        write(connection->getDescription());
        write("It leads to " + connection->getOtherRoom(getAdventure()->getPlayer()->currentRoom())->getName() + ".");
    }
    else
    {
        ErrorDialog("This shouldn't have happened", "Location can't be unlocked");
    }
}