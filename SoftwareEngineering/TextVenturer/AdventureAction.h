#pragma once

#include "BaseAction.h"

class BaseAction;
class Adventure;
class Player;
class Inventory;
class Room;
class Location;
class ItemCombiner;

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

