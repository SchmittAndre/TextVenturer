#pragma once

#include "Location.h"

class RoomConnection : public Location
{
private:
    Room* room1;
    Room* room2;

    bool accessible;     
    std::string lockedDescription;

public:                 
    RoomConnection(const std::string & name, const std::string & description, Room* room1, Room* room2, bool accessible = true, const std::string & lockedDescription = "");

    Room* getOtherRoom(const Room* room) const;
    bool isAccessible() const;

    void lock();
    void unlock();

    std::string getDescription() const;
    void changeLockedDescription(const std::string & description);
};

