#pragma once

#include "Inventory.h"

class Room;
class Location;
class AdventureObject;
class CommandSystem;
class Item;

class Player
{
private:

    class PlayerInventory : public Inventory
    {
    private:
        Player & player;

    public:
        PlayerInventory(FileStream & stream, AdventureLoadHelp & help, Player & player);
        PlayerInventory(Player & player);

        void addItem(Item & item);
        void delItem(Item & item);  
    };

    // order specific for load-constructor intializer-list
    std::string name;
    Room & room;
    Location * location;

    CommandSystem & commandSystem;
    PlayerInventory inventory;
    
    std::unordered_set<AdventureObject*> knownSubjects;

public:
    Player(FileStream & stream, CommandSystem & commandSystem, AdventureLoadHelp & help);
    Player(std::string name, Room & startroom, CommandSystem & commandSystem);
    virtual ~Player();

    void gotoLocation(Location & location);
    void leaveLocation();

    void gotoRoom(Room & room);

    Inventory & getInventory();
    Room & currentRoom();
    Location & currentLocation();
    bool isAtLocation() const;

    bool knows(const AdventureObject & subject) const;
    void inform(AdventureObject & subject);
    void forget(AdventureObject & subject);

    std::string getName() const;
    void rename(std::string name); 

    CommandSystem & getCommandSystem();

    void save(FileStream & stream, AdventureSaveHelp & help) const;
};

class ENotAtLocation : public Exception
{
public:
    ENotAtLocation();
};