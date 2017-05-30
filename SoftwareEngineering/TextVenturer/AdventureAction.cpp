#include "stdafx.h"     

#include "CmdLine.h"
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

AdventureAction::AdventureAction(Adventure & adventure)
    : adventure(adventure)
{
}

AdventureAction::~AdventureAction()
{
}

CmdLine & AdventureAction::getCmdLine() 
{
    return adventure.getCmdLine();
}

void AdventureAction::write(const std::string & text)
{
    getCmdLine().write(text);
}

Adventure & AdventureAction::getAdventure() const
{
    return adventure;
}

Player & AdventureAction::getPlayer() const
{
    return adventure.getPlayer();
}

Inventory & AdventureAction::getPlayerInv() const
{
    return getPlayer().getInventory();
}

Room & AdventureAction::currentRoom() const
{
    return getPlayer().currentRoom();
}

Location & AdventureAction::currentLocation() const
{
    return getPlayer().currentLocation();
}

ItemCombiner & AdventureAction::getItemCombiner() const
{
    return adventure.getItemCombiner();
}

void AdventureAction::changeRoom(RoomConnection & connection, bool showDescription)
{
    Room & fromRoom = currentRoom();
    Room & toRoom = connection.getOtherRoom(fromRoom);
    
    leaveLocation();

    // all actions have no action or action does not override
    if ((!fromRoom.getOnLeave() || !fromRoom.getOnLeave()->overrides()) &&
        (!connection.getOnUse() || !connection.getOnUse()->overrides()) &&
        (!toRoom.getOnEnter() || !toRoom.getOnEnter()->overrides()))
    {
        getPlayer().inform(connection);
        getPlayer().inform(toRoom);
        getPlayer().gotoRoom(toRoom);
        write("You went through " + connection.getName(getPlayer()) + " and entered " + toRoom.getName(getPlayer()) + ".");
        if (showDescription)
            write(toRoom.getDescription());
    }
    
    if (fromRoom.getOnLeave())
        fromRoom.getOnLeave()->run();

    if (connection.getOnUse())
        connection.getOnUse()->run();

    if (toRoom.getOnEnter())
        toRoom.getOnEnter()->run();
}

void AdventureAction::leaveLocation()
{
    if (getPlayer().isAtLocation())
    {
        Location & oldLocation = currentLocation();

        if (oldLocation.getOnLeave())
            oldLocation.getOnLeave()->run();           
    }
}

void AdventureAction::changeLocation(Location & location, bool showDescription) 
{
    Location * oldLocation; 
    if (getPlayer().isAtLocation())
        oldLocation = &currentLocation();
    else
        oldLocation = NULL;

    bool atLocationAlready = oldLocation == &location;
   
    if (atLocationAlready ||
        (!oldLocation || !oldLocation->getOnLeave() || !oldLocation->getOnLeave()->overrides()) &&
        (!location.getOnGoto() || !location.getOnGoto()->overrides()))
    {        
        if (!atLocationAlready)
        {
            getPlayer().inform(location);
            getPlayer().gotoLocation(location);
            write("You went to " + location.getName(getPlayer()) + ".");
        }
        if (showDescription)
        {
            write(location.getDescription());
        }        
    }

    if (atLocationAlready)
        return;

    if (oldLocation && oldLocation->getOnLeave())
        oldLocation->getOnLeave()->run();

    if (location.getOnGoto())
        location.getOnGoto()->run();
}

void AdventureAction::combine(Item & item1, Item & item2, Item & result)
{
    CustomAdventureAction* action = getItemCombiner().getOnCombine(item1, item2);
    
    if (!action || !action->overrides())
    {
        getPlayerInv().delItem(item1);
        getPlayerInv().delItem(item2);
        getPlayerInv().addItem(result);
        write("You combined the two and received " + result.getName(getPlayer()) + ".");
        getPlayer().inform(result);
    }

    if (action)
        action->run();             
}

void AdventureAction::take(Location::MultiInventory & inventory, Item & item)
{
    if (!item.getOnTake() || !item.getOnTake()->overrides())
    {
        inventory.delItem(item);
        getPlayerInv().addItem(item);
        getPlayer().inform(item);
        write("You took " + item.getName(getPlayer()) +
            " " + inventory.getPrepositionName(true) +
            " " + currentLocation().getName(getPlayer()) + ".");
    }

    if (item.getOnTake())
        item.getOnTake()->run();
}

void AdventureAction::place(Location::MultiInventory & inventory, Item & item)
{
    if (!item.getOnPlace() || !item.getOnPlace()->overrides())
    {
        getPlayerInv().delItem(item);
        write("You placed " + item.getName(getPlayer()) + 
            " " + inventory.getPrepositionName() + 
            " " + currentLocation().getName(getPlayer()) + ".");
    }

    if (item.getOnPlace())
        item.getOnPlace()->run();          
}

void AdventureAction::inspect(AdventureObject & object)
{
    if (!object.getOnInspect() || !object.getOnInspect()->overrides())
    {
        getPlayer().inform(object);
        write(object.getDescription());
        try
        {
            Location & location = dynamic_cast<Location&>(object);
            if (location.filledInventoryCount() > 0)
            {
                std::string content;
                auto invs = location.getInventories();
                for (auto inv = invs.begin(); inv != invs.end(); inv++)
                {
                    if (inv->get().isEmpty())
                        continue;
                    if (inv == invs.end() - 1 && content != "")
                        content += " and ";
                    content += inv->get().formatContents(getPlayer()) +
                        " " + inv->get().getPrepositionName() +
                        " " + location.getName(getPlayer());
                    if (invs.size() > 1 && inv < invs.end() - 2)
                        content += ", ";

                    for (Item & item : inv->get().getItems())
                        getPlayer().inform(item);
                }
                std::string be = location.firstFilledInventory().getItemCount() > 1 ||
                    location.firstFilledInventory().getItems()[0].get().isNamePlural() ? "are " : "is ";
                write("There " + be + content + ".");
            }
        }
        catch (std::bad_cast) { }

        try
        {
            RoomConnection & connection = dynamic_cast<RoomConnection&>(object);
            if (connection.isAccessible())
            {
                write(connection.getName(getPlayer(), true) + " leads to " + connection.getOtherRoom(currentRoom()).getName(getPlayer()) + ".");
            }
        }
        catch (std::bad_cast) { }
    }
    
    if (object.getOnInspect())
        object.getOnInspect()->run();
}

