#pragma once

#include "Location.h"

class RoomConnection : public Location
{
private:
    Room* room1;
    Room* room2;

    bool accessible;     

public:                 
    RoomConnection(Room* room1, Room* room2, bool accessible = true);

    Room* getOtherRoom(const Room* room) const;
    bool isAccessible() const;

    void lock();
    void unlock();
};

