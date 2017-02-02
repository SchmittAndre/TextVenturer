#pragma once

#include "Location.h"

class RoomConnection : public Location
{
private:
    Room* room1;
    Room* room2;

    bool accessible;     
    string lockedDescription;

public:                 
    RoomConnection(const string & name, const string & description, Room* room1, Room* room2, bool accessible = true, const string & lockedDescription = "");

    Room* getOtherRoom(const Room* room) const;
    bool isAccessible() const;

    void lock();
    void unlock();

    string getDescription() const;
    void changeLockedDescription(const string & description);
};

