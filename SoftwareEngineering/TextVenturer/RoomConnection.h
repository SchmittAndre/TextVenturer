#pragma once

#include "Location.h"

class Room;

class RoomConnection : public Location
{
private:
    Room* room;
    bool accessible;

    string lockedDescription;

public:                 
    RoomConnection(string name, string description, Room* room, bool accessible = true, string lockedDescription = "");

    Room* getRoom();
    bool isAccessible();

    void lock();
    void unlock();

    string getDescription();
    void changeLockedDescription(string description);
};

