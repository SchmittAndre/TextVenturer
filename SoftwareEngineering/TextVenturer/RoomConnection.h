#pragma once

#include "Location.h"

class Room;
class CustomAdventureAction;

class RoomConnection : public Location
{
private:
    Room* room1;
    Room* room2;

    bool accessible;     

    CustomAdventureAction* onUse;

public:                 
    RoomConnection(Room* room1, Room* room2, bool accessible = true);
    ~RoomConnection();

    Room* getOtherRoom(const Room* room) const;
    bool isAccessible() const;

    void lock();
    void unlock();

    CustomAdventureAction* getOnUse();
    void setOnUse(CustomAdventureAction* onUse);
};

