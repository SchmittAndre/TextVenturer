#pragma once

class Inventory;
class Room;
class Location;

class Player
{
private:
    string name;

    Inventory* inventory;
    Room* room;
    Location* location;

    unordered_set<void*> knownSubjects;

public:
    Player(string name, Room* startroom);
    virtual ~Player();

    void gotoLocation(Location* location);
    void gotoRoom(Room* room);

    Inventory* getInventory() const;
    Room* currentRoom() const;
    Location* currentLocation() const;
    bool isAtLocation() const;

    bool knows(void* subject) const;
    void inform(void* subject);
    void forget(void* subject);

    string getName() const;
    void rename(string name); 
};

