#pragma once

class Adventure;
class Player;
class Room;
class Location;
class Inventory;
class ItemCombiner;

#include "BaseAction.h"

class AdventureAction : public BaseAction
{
private:
    Adventure* adventure;
public:
    AdventureAction(Adventure* adventure);

    Adventure* getAdventure() const;
    Player* getPlayer() const;
    Inventory* getPlayerInv() const;
    Room* currentRoom() const;
    Location* currentLocation() const;
    ItemCombiner* getItemCombiner() const;
};

