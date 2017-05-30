#pragma once

#include "Location.h"
                     
class AdventureObject;
class Adventure;
class Player;
class Inventory;
class Room;
class ItemCombiner;
class RoomConnection;
class Item;
class CmdLine;

class AdventureAction
{
private:
    Adventure & adventure;

public:
    AdventureAction(Adventure & adventure);
    virtual ~AdventureAction();

    CmdLine & getCmdLine();                     
    virtual bool run(const Command::Result & params = Command::Result()) = 0;
    void write(const std::string & text);
    virtual taglist requiredParameters() const { return{}; };            

    // getter
    Adventure & getAdventure() const;
    Player & getPlayer() const;
    Inventory & getPlayerInv() const;
    Room & currentRoom() const;
    Location & currentLocation() const;
    ItemCombiner & getItemCombiner() const;

    // helping functions
    void changeRoom(RoomConnection & connection, bool showDescription);
    void leaveLocation();
    void changeLocation(Location & location, bool showDescription);
    void combine(Item & item1, Item & item2, Item & result);
    void take(Location::MultiInventory & inventory, Item & item);   
    void place(Location::MultiInventory & inventory, Item & item);
    void inspect(AdventureObject & object); 
};

