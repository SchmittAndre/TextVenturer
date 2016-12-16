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
    Player(string name);
    ~Player();

    void gotoLocation(Location* location);
    void gotoRoom(Room* room);

    Inventory* getInventory();
    Room* currentRoom();
    Location* currentLocation();
    bool isAtLocation();

    string getName();
    void rename(string name); 
};

