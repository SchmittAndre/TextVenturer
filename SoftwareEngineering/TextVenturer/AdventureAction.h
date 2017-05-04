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
    Adventure* adventure;

public:
    AdventureAction(Adventure* adventure);
    virtual ~AdventureAction();

    CmdLine* getCmdLine() const;                     
    virtual bool run(const Command::Result & params = Command::Result()) = 0;
    void write(const std::string & text) const;
    virtual taglist requiredParameters() const { return{}; };            

    // getter
    Adventure* getAdventure() const;
    Player* getPlayer() const;
    Inventory* getPlayerInv() const;
    Room* currentRoom() const;
    Location* currentLocation() const;
    ItemCombiner* getItemCombiner() const;

    // helping functions
    bool changeRoom(RoomConnection* connection, bool showDescription) const;
    bool changeLocation(Location* location, bool showDescription) const;
    void combine(Item* item1, Item* item2, Item* result) const;
    void take(Location::PInventory* inventory, Item* item) const;   
    void place(Location::PInventory* inventory, Item* item) const;
    void inspect(AdventureObject* object) const; 
};

