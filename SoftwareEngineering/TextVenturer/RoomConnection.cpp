#include "stdafx.h"

#include "RoomConnection.h"

RoomConnection::RoomConnection(const string & name, const string & description, Room* room1, Room* room2, bool accessible, const string & lockedDescription)
:   Location(name, description)
{
    this->room1 = room1;
    this->room2 = room2;
    this->accessible = accessible;
    this->lockedDescription = lockedDescription;
}

Room * RoomConnection::getOtherRoom(const Room* room) const
{
    if (room1 == room)
        return room2;
    if (room2 == room)
        return room1;
    return NULL;
}

bool RoomConnection::isAccessible() const
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

string RoomConnection::getDescription() const
{
    return accessible ? Location::getDescription() : lockedDescription;
}

void RoomConnection::changeLockedDescription(const string & description)
{
    lockedDescription = description;
}
