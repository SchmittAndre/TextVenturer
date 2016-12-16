#include "stdafx.h"
#include "Room.h"

#include "RoomConnection.h"

RoomConnection::RoomConnection(string name, string description, Room* room1, Room* room2, bool accessible, string lockedDescription)
:   Location(name, description)
{
    this->room1 = room1;
    this->room2 = room2;
    this->accessible = accessible;
}

Room * RoomConnection::getOtherRoom(Room* room)
{
    if (room1 == room)
        return room2;
    if (room2 == room)
        return room1;
    return NULL;
}

bool RoomConnection::isAccessible()
{
    return accessible;
}

void RoomConnection::lock()
{
    accessible = false;
}

void RoomConnection::unlock()
{
    accessible = true;
}

string RoomConnection::getDescription()
{
    if (accessible)
        return Location::getDescription();
    return lockedDescription;
}

void RoomConnection::changeLockedDescription(string description)
{
    lockedDescription = description;
}
