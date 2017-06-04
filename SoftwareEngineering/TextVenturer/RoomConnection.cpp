#include "stdafx.h"

#include "Room.h"
#include "CommandSystem.h"
#include "CustomAdventureAction.h"
#include "Adventure.h"

#include "RoomConnection.h"

AdventureObject::Type RoomConnection::getType() const
{
    return otRoomConnection;
}

RoomConnection::RoomConnection(Room & room1, Room & room2, bool accessible)
    : room1(room1)
    , room2(room2)
    , accessible(accessible)
    , onUse(NULL)
{
    room1.addLocation(*this);
    room2.addLocation(*this);
}      

RoomConnection::RoomConnection(FileStream & stream, AdventureLoadHelp & help)
    : Location(stream, help)
    , room1(dynamic_cast<Room&>(help.objects[stream.readUInt()].get()))
    , room2(dynamic_cast<Room&>(help.objects[stream.readUInt()].get()))
    , accessible(stream.readBool())
    , onUse(CustomAdventureAction::loadConditional(stream, help.adventure))
{
    room1.addLocation(*this);
    room2.addLocation(*this);
}

RoomConnection::~RoomConnection()
{
    delete onUse;
}

Room & RoomConnection::getOtherRoom(Room & room) const
{
    if (&room1 == &room)
        return room2;
    if (&room2 == &room)
        return room1;
    throw(EInvalidConnectionRoom, *this, room);
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

CustomAdventureAction* RoomConnection::getOnUse() const
{
    return onUse;
}

void RoomConnection::setOnUse(CustomAdventureAction * onUse)
{
    this->onUse = onUse;
}

void RoomConnection::save(FileStream & stream, AdventureSaveHelp & help) const
{
    Location::save(stream, help);
    stream.write(help.objects[&room1]);
    stream.write(help.objects[&room2]);
    stream.write(accessible);
    CustomAdventureAction::saveConditional(stream, onUse);
}

EInvalidConnectionRoom::EInvalidConnectionRoom(const RoomConnection & connection, const Room & room)
    : Exception("Connection \"" + connection.getNameOnly() + "\" does not connect the room \"" + room.getNameOnly() + "\"")
{
}
