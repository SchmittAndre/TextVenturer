#pragma once

#include "BaseAction.h"
#include "Location.h"
                     
class AdventureObject;
class Adventure;
class Player;
class Inventory;
class Room;
class ItemCombiner;
class RoomConnection;
class Item;

class AdventureAction : public BaseAction
{
private:
    Adventure* adventure;

public:
    AdventureAction(Adventure* adventure);

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

