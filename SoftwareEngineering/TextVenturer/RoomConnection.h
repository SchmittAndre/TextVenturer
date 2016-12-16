#pragma once

#include "Location.h"

class Room;

class RoomConnection : public Location
{
private:
    Room* room1;
    Room* room2;

    bool accessible;

    string lockedDescription;

public:                 
    RoomConnection(string name, string description, Room* room1, Room* room2, bool accessible = true, string lockedDescription = "");

    Room* getOtherRoom(Room* room);
    bool isAccessible();

    void lock();
    void unlock();

    string getDescription();
    void changeLockedDescription(string description);
};

