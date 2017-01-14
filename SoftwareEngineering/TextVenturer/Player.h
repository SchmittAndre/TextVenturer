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

public:
    Player(string name, Room* startroom);
    virtual ~Player();

    void gotoLocation(Location* location);
    void gotoRoom(Room* room);

    Inventory* getInventory() const;
    Room* currentRoom() const;
    Location* currentLocation() const;
    bool isAtLocation() const;

    string getName() const;
    void rename(string name); 
};

