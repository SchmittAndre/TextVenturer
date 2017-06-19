#include "stdafx.h"

#include "AliasList.h"
#include "Adventure.h"
#include "Room.h"
#include "Player.h"
#include "RoomConnection.h"
#include "Item.h"
#include "ItemCombiner.h"
#include "CustomAdventureAction.h"
#include "Controler.h"
#include "CmdLine.h"
#include "Game.h"
#include "Window.h"

#include "DefaultAdventureAction.h"

bool DefaultAdventureAction::run(const Command::Result & params) 
{
    // The input does not match with any available command
    write("I didn't quite get that...");
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
    write("You can see " + currentRoom().formatLocations(getPlayer()) + ".");
    for (Location & location : currentRoom().getLocations())
        getPlayer().inform(location);
    return true;
}

bool ShowInventoryAction::run(const Command::Result & params) 
{
    // List the players inventory
    if (getPlayerInv().isEmpty())
    {
        write("You are not carrying anything.");
    }
    else
    {
        write("You are carrying " + getPlayerInv().formatContents(getPlayer()) + ".");
        for (Item & item : getPlayerInv().getItems())
            getPlayer().inform(item);
    }
    return true;
}

bool InspectAction::run(const Command::Result & params) 
{
    // Inspect a location and show the description, also goes to the location
    if (getPlayer().isAtLocation())
    {
        try
        {
            Item & item = currentLocation().findItem(params["thing"]);
            inspect(item);
            return true;
        }
        catch (EItemNotFound) { }     
    }

    try
    {
        RoomConnection & connection = currentRoom().findRoomConnectionTo(params["thing"]);
        if (changeRoom(connection))
            inspect(currentRoom());
        return true;
    }
    catch (ERoomNotFound) { }

    if (currentRoom().getAliases().has(params["thing"]))
    {
        inspect(currentRoom());
        return true;
    }

    try
    {
        Location & location = currentRoom().findLocation(params["thing"]);
        if (changeLocation(location))
            inspect(location);
        return true;
    }
    catch (ELocationNotFound) { }

    try
    {
        Item & item = getPlayerInv().findItem(params["thing"]);
        inspect(item);
        return true;
    }
    catch (EItemNotFound) { }
    
    write("I didn't get, what you want to investigate.");
    return true;
}

bool TakeFromAction::run(const Command::Result & params) 
{
    // Take an item from a location using a preposition
    try
    {
        Location & location = currentRoom().findLocation(params["location"]);

        if (changeLocation(location))
        {
            Location::MultiInventory * lastMatch = NULL;
            for (Location::MultiInventory & inv : location.findInventories(params["prep"], true))
            {
                lastMatch = &inv;
                try
                {
                    Item & item = inv.findItem(params["item"]);
                    {
                        take(inv, item);
                        return true;
                    }
                }
                catch (EItemNotFound) {}
            }

            if (!lastMatch)
            {
                if (location.filledInventoryCount() > 0)
                {
                    write("There is only something " + location.formatPrepositions(true) + " " + location.getName(getPlayer()) + ".");
                }
                else
                {
                    write("There is nothing at " + location.getName(getPlayer()) + ".");
                }
            }
            else
            {
                write("There is no " + Alias(params["item"]).nameOnly() + " " + lastMatch->getPrepositionName() + " " + location.getName(getPlayer()) + ".");
            }
        }
        return true;
    }
    catch (ELocationNotFound) { }

    write("I don't get where you are trying to go.");
    return true;
}

bool TakeAction::run(const Command::Result & params) 
{
    // Take an item from a location
    try
    {
        Location & location = currentRoom().findLocation(params["item"]);
        getPlayer().inform(location);
        write("You can't pick up " + location.getName(getPlayer()) + ".");
        return true;
    }
    catch (ELocationNotFound) { }

    if (currentRoom().getAliases().has(params["item"]))
    {
        write("Are you crazy? You can't pick up " + currentRoom().getName(getPlayer()) + ".");
        return true;
    }
    
    try
    {
        Room & room = currentRoom().findRoom(params["item"]);
        getPlayer().inform(room);
        write("Are you insane? You can't pick up " + room.getName(getPlayer()) + ".");
        return true;
    }
    catch (ERoomNotFound) { }

    if (getPlayer().isAtLocation())
    {
        for (Location::MultiInventory & inv : currentLocation().getInventories())
        {
            try
            {
                Item & item = inv.findItem(params["item"]);
                {
                    take(inv, item);
                    return true;
                }
            }
            catch (EItemNotFound) {}
        }
    }

    write("I don't get what you are trying to pick up.");
    return true;
}

bool PlaceAction::run(const Command::Result & params) 
{
    // Place down an item at a specific preposition at a location
    try
    {
        Item & item = getPlayerInv().findItem(params["item"]);
        getPlayer().inform(item);

        try
        {
            Location & location = currentRoom().findLocation(params["location"]);
        
            if (changeLocation(location))
            {

                Location::MultiInventory * filterFailure = NULL;
                for (Location::MultiInventory & inv : location.findInventories(params["prep"]))
                {
                    try
                    {
                        inv.addItem(item);
                        place(inv, item);
                        return true;
                    }
                    catch (EAddItemFilterForbidden)
                    {
                        filterFailure = &inv;
                        continue;
                    }
                }

                if (filterFailure)
                    write("You can't put " + item.getName(getPlayer()) + " " + filterFailure->getPrepositionName() + " " + location.getName(getPlayer()) + ".");
                else
                    write("You can only put " + item.getName(getPlayer()) +
                        " " + location.formatPrepositions(item) +
                        " " + location.getName(getPlayer()));
            }
            return true;
        }
        catch (ELocationNotFound) { }
        write("I don't get where you are trying to put " + item.getName(getPlayer()) + ".");
        return true;
    }
    catch (EItemNotFound) { }     
    write("You don't have that item, you should check your inventory.");
    return true;
}

bool UseRoomConnectionAction::run(const Command::Result & params) 
{
    // Use a room connection to get to another room if it is accessible
    try
    {
        Location & location = currentRoom().findLocation(params["door"]);
        getPlayer().inform(location);
        try
        {
            RoomConnection & connection = dynamic_cast<RoomConnection&>(location);
        
            if (connection.isAccessible())
            {
                changeRoom(connection);    
            }
            else
            {
                write(connection.getDescription());
            }
            return true;
        }
        catch (std::bad_cast) { }
        write("You can't go through " + location.getName(getPlayer()) + ".");
        return true;
    }
    catch (ELocationNotFound) { }

    if (currentRoom().getAliases().has(params["door"]))
    {
        inspect(currentRoom());
        return true;
    }

    try
    {
        RoomConnection& connection = currentRoom().findRoomConnectionTo(params["door"]);
        if (changeRoom(connection))
            inspect(currentRoom());
        return true;
    }
    catch (ERoomNotFound) { }

    write("I don't get where you are trying to go.");
    return true;
}

bool GotoAction::run(const Command::Result & params) 
{
    // Go to a specific location in the current room
    try
    {
        Location & location = currentRoom().findLocation(params["place"]);
        if (getPlayer().isAtLocation() && &currentLocation() == &location)
        {
            getPlayer().inform(location);
            write("You are at " + location.getName(getPlayer()) + " already.");
        }
        else
        {
            changeLocation(location);
        }
        return true;
    }
    catch (ELocationNotFound) { }

    if (currentRoom().getAliases().has(params["place"]))
    {
        write("You are in " + currentRoom().getName(getPlayer()) + " already.");
        return true;
    }
    
    try
    {
        RoomConnection & connection = currentRoom().findRoomConnectionTo(params["place"]);
        changeRoom(connection);
        return true;
    }
    catch (ERoomNotFound) { }
    write("I don't get where you are trying to go.");
    return true;
}

bool EnterRoomAction::run(const Command::Result & params) 
{
    // Enter a room if it is accessible
    if (getPlayer().isAtLocation())
    {
        try
        {
            Item & item = currentLocation().findItem(params["room"]);
            getPlayer().inform(item);
            write("You can't enter " + item.getName(getPlayer()) + ".");
            return true;
        }
        catch (EItemNotFound) { }
    }

    if (currentRoom().getAliases().has(params["room"]))
    {
        write("You are in " + currentRoom().getName(getPlayer()) + " already.");
        return true;
    }

    try
    {
        RoomConnection & connection = currentRoom().findRoomConnectionTo(params["room"]);

        if (connection.isAccessible())
        {
            changeRoom(connection);
        }
        else
        {
            write(connection.getDescription());
        }
        return true;
    }
    catch (ERoomNotFound) { }

    try
    {
        Location & location = currentRoom().findLocation(params["room"]);
        getPlayer().inform(location);
        write("You can't enter " + location.getName(getPlayer()) + ".");
        return true;
    }
    catch (ELocationNotFound) { }
    write("Here is no " + Alias(params["room"]).nameOnly() + ".");
    return true;
}

bool CombineItemsAction::run(const Command::Result & params) 
{
    // Combine two items from the players inventory if possible
    Item * item1;
    Item * item2;
    try
    {
        item1 = &getPlayerInv().findItem(params["item1"]);
    }
    catch (EItemNotFound) 
    {
        item1 = NULL;
    }

    try
    {
        item2 = &getPlayerInv().findItem(params["item2"]);
    }
    catch (EItemNotFound)
    {
        item2 = NULL;
    }

    if (item1 && item2)
    {
        if (item1 == item2)
        {
            write("You can't combine " + item1->getName(getPlayer()) + " with itself!");
            return true;
        }

        try
        {
            Item & result = getItemCombiner().getResult(*item1, *item2);
            combine(*item1, *item2, result);
        }
        catch (ECombinationDoesNotExists) 
        {                                                                                                            
            write("You can't combine " + item1->getName(getPlayer()) + " with " + item2->getName(getPlayer()) + ".");
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

bool ExitAction::run(const Command::Result & params)
{
    getAdventure().getCmdLine().getControler().changeDisplayer(Controler::dtMainMenu);
	return true;
}     
    