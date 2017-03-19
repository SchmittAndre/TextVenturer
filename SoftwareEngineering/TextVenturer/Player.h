#pragma once

class Inventory;
class Room;
class Location;
class AdventureObject;
class CommandSystem;

class Player
{
private:
    std::string name;

    CommandSystem* commandSystem;
    Inventory* inventory;
    Room* room;
    Location* location;

    std::unordered_set<AdventureObject*> knownSubjects;

public:
    Player(FileStream & stream, CommandSystem* commandSystem, std::vector<AdventureObject*> objectList);
    Player(std::string name, Room* startroom, CommandSystem* commandSystem);
    virtual ~Player();

    void gotoLocation(Location* location);
    void gotoRoom(Room* room);

    Inventory* getInventory() const;
    Room* currentRoom() const;
    Location* currentLocation() const;
    bool isAtLocation() const;

    bool knows(AdventureObject* subject) const;
    void inform(AdventureObject* subject);
    void forget(AdventureObject* subject);

    std::string getName() const;
    void rename(std::string name); 

    CommandSystem* getCommandSystem() const;

    void save(FileStream & stream, idlist<AdventureObject*> objectIDs);
};

