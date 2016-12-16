#include "stdafx.h"
#include "Room.h"

#include "RoomConnection.h"

RoomConnection::RoomConnection(string name, string description, Room* room, bool accessible, string lockedDescription)
:   Location(name, description)
{
    this->room = room;
    this->accessible = accessible;
}

Room * RoomConnection::getRoom()
{
    return room;
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
