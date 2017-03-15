#include "stdafx.h"

#include "Adventure.h"
#include "Room.h"
#include "Player.h"
#include "RoomConnection.h"
#include "Item.h"
#include "ItemCombiner.h"
#include "CustomAdventureAction.h"

#include "DefaultAdventureAction.h"

bool DefaultAdventureAction::run(const Command::Result & params) 
{
    // The input does not match with any available command
    write("I didn't quiet get that...");
    return true;
}

bool HelpAction::run(const Command::Result & params) 
{
    // Give the player some helpful advice               
    write("I'm sorry, but I can't help you at the moment!");
    return true;
}

bool LookAroundAction::run(const Command::Result & params) 
{
    // Look around and list all locations
    write("You can see " + currentRoom()->formatLocations(getPlayer()) + ".");
    for (Location* location : currentRoom()->getLocations())
        getPlayer()->inform(location);
    return true;
}

bool ShowInventoryAction::run(const Command::Result & params) 
{
    // List the players inventory
    if (getPlayerInv()->isEmpty())
    {
        write("You are not carrying anything.");
    }
    else
    {
        write("You are carrying " + getPlayerInv()->formatContents() + ".");
    }
    return true;
}

bool InspectAction::run(const Command::Result & params) 
{
    // Inspect a location and show the description, also goes to the location
    if (getPlayer()->isAtLocation())
    {
        for (Location::PInventory* inv : currentLocation()->getInventories())
        {
            if (Item* item = inv->findItem(params["thing"]))
            {
                inspect(item);
                return true;
            }
        }
    }

    if (RoomConnection* connection = currentRoom()->findRoomConnectionTo(params["thing"]))
    {
        changeRoom(connection, false);
        inspect(currentRoom());
    }
    else if (currentRoom()->getAliases().has(params["thing"]))
    {
        inspect(currentRoom());
    }
    else if (Location* location = currentRoom()->findLocation(params["thing"]))
    {
        changeLocation(location, false);
        inspect(location);
    }
    else if (Item* item = getPlayerInv()->findItem(params["thing"]))
    {
        inspect(item);
    }
    else
    {
        write("Where exactly is that, if I shall ask?");
    }
    return true;
}

bool TakeFromAction::run(const Command::Result & params) 
{
    // Take an item from a location using a preposition
    if (Location* location = currentRoom()->findLocation(params["location"]))
    {
        if (location == currentLocation() || changeLocation(location, false))
        {
            Location::PInventory* lastMatch = NULL;
            for (Location::PInventory* inv : location->getInventories())
            {
                if (inv->hasPrepositionAlias(params["prep"], true))
                {
                    lastMatch = inv;
                    if (Item* item = inv->findItem(params["item"]))
                    {
                        take(inv, item);
                        return true;
                    }
                }
            }
            if (!lastMatch)
            {
                if (location->filledInventoryCount() > 0)
                {
                    write("There is only something " + location->formatPrepositions(true) + " " + location->getName(getPlayer()) + ".");
                }
                else
                {
                    write("There is nothing at " + location->getName(getPlayer()) + ".");
                }
            }
            else
            {
                write("There is no " + Alias(params["item"]).nameOnly() + " " + lastMatch->getPrepositionName() + " " + location->getName(getPlayer()) + ".");
            }
        }
    }
    else
    {
        write("There is no " + Alias(params["location"]).nameOnly() + " here.");
    }
    return true;
}

bool TakeAction::run(const Command::Result & params) 
{
    // Take an item from a location
    if (Location* location = currentRoom()->findLocation(params["item"]))
    {
        getPlayer()->inform(location);
        write("You can't pick up " + location->getName(getPlayer()) + ".");
    }
    else if (currentRoom()->getAliases().has(params["item"]))
    {
        write("Are you crazy? You can't pick up " + currentRoom()->getName(getPlayer()) + ".");
    }
    else if (Room* room = currentRoom()->findRoom(params["item"]))
    {
        getPlayer()->inform(room);
        write("Are you insane? You can't pick up " + room->getName(getPlayer()) + ".");
    }
    else if (getPlayer()->isAtLocation())
    {
        for (Location::PInventory* inv : currentLocation()->getInventories())
        {
            if (Item* item = inv->findItem(params["item"]))
            {
                take(inv, item);
                return true;
            }
        }
        write("Here is no " + Alias(params["item"]).nameOnly() + ".");
    }
    else
    {
        write("There is no " + Alias(params["item"]).nameOnly() + " here.");
    }
    return true;
}

bool PlaceAction::run(const Command::Result & params) 
{
    // Place down an item at a specific preposition at a location
    if (Item* item = getPlayerInv()->findItem(params["item"]))
    {
        getPlayer()->inform(item);
        if (Location* location = currentRoom()->findLocation(params["location"]))
        {
            if (location == currentLocation() || changeLocation(location, false))
            {
                Location::PInventory* whitelistFailure = NULL;
                for (auto inv : location->getInventories())
                {
                    if (inv->hasPrepositionAlias(params["prep"]))
                    {
                        if (!inv->addItem(item))
                        {
                            whitelistFailure = inv;
                            continue;
                        }
                        place(inv, item);
                        return true;
                    }
                }

                if (whitelistFailure)
                    write("You can't put " + item->getName(true) + " " + whitelistFailure->getPrepositionName() + " " + location->getName(getPlayer()) + ".");
                else
                    write("You can only put something " + location->formatPrepositions() + " " + location->getName(getPlayer()));
            }
        }
        else
        {
            write("Here is no " + Alias(params["location"]).nameOnly() + ".");
        }
    }
    else
    {
        write("You have no " + Alias(params["item"]).nameOnly() + ".");
    }
    return true;
}

bool UseRoomConnectionAction::run(const Command::Result & params) 
{
    // Use a room connection to get to another room if it is accessible
    if (Location* location = currentRoom()->findLocation(params["door"]))
    {
        if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        {
            if (connection->isAccessible())
            {
                changeRoom(connection, false);    
            }
            else
            {
                write(connection->getDescription());
            }
        }
        else
        {
            write("You can't go through " + location->getName(getPlayer()) + ".");
        }
    }
    else if (Room* room = currentRoom()->findRoom(params["door"]))
    {
        write("You can't go through " + room->getName(getPlayer()) + ".");
    }
    else
    {
        write("Where is that?");
    }
    return true;
}

bool GotoAction::run(const Command::Result & params) 
{
    // Go to a specific location in the current room
    if (Location* location = currentRoom()->findLocation(params["place"]))
    {
        if (currentLocation() == location)
        {
            getPlayer()->inform(location);
            write("You are at " + location->getName(getPlayer()) + " already.");
        }
        else
        {
            changeLocation(location, false);
        }
    }
    else if (currentRoom()->getAliases().has(params["place"]))
    {
        write("You are in " + currentRoom()->getName(getPlayer()) + " already.");
    }
    else if (RoomConnection* connection = currentRoom()->findRoomConnectionTo(params["place"]))
    {
        changeRoom(connection, false);
    }
    else
    {
        write("Here is no " + Alias(params["place"]).nameOnly() + ".");
    }
    return true;
}

bool EnterRoomAction::run(const Command::Result & params) 
{
    // Enter a room if it is accessible
    if (currentRoom()->getAliases().has(params["room"]))
    {
        write("You are in " + currentRoom()->getName(getPlayer()) + " already.");
    }
    else if (RoomConnection* connection = currentRoom()->findRoomConnectionTo(params["room"]))
    {
        if (connection->isAccessible())
        {
            changeRoom(connection, false);
        }
        else
        {
            write(connection->getDescription());
        }
    }   
    else
    {
        write("Here is no " + Alias(params["room"]).nameOnly() + ".");
    }
    return true;
}

bool CombineItemsAction::run(const Command::Result & params) 
{
    // Combine two items from the players inventory if possible
    Item* item1 = getPlayerInv()->findItem(params["item1"]);
    Item* item2 = getPlayerInv()->findItem(params["item2"]);
    if (item1 && item2)
    {
        if (item1 == item2)
        {
            write("You can't combine " + item1->getName(true) + " with itself!");
        }
        else if (Item* result = getItemCombiner()->getResult(item1, item2))
        {
            combine(item1, item2, result);
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
    return true;
}

