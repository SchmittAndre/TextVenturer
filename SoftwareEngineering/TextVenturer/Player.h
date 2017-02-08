#pragma once

class Inventory;
class Room;
class Location;
class AdventureObject;

class Player
{
private:
    std::string name;

    Inventory* inventory;
    Room* room;
    Location* location;

    std::unordered_set<AdventureObject*> knownSubjects;

public:
    Player(std::string name, Room* startroom);
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
};

