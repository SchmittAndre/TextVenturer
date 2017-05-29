#include "stdafx.h"

#include "Room.h"
#include "CommandSystem.h"
#include "CustomAdventureAction.h"

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
}      

RoomConnection::RoomConnection(FileStream & stream, Adventure & adventure, ref_vector<AdventureObject> & objectList, ref_vector<CommandArray> & commandArrayList)
    : Location(stream, adventure)
    , room1(dynamic_cast<Room&>(objectList[stream.readUInt()].get()))
    , room2(dynamic_cast<Room&>(objectList[stream.readUInt()].get()))
{
    Location::load(stream, adventure, objectList, commandArrayList);
    stream.read(accessible);
    loadAdventureAction(stream, adventure, onUse);
}

RoomConnection::~RoomConnection()
{
    delete onUse;
}

Room & RoomConnection::getOtherRoom(const Room & room) const
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

void RoomConnection::save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs, ref_idlist<CommandArray> & commandArrayIDs) const
{
    Location::save(stream, objectIDs, commandArrayIDs);
    stream.write(objectIDs[room1]);
    stream.write(objectIDs[room2]);
    stream.write(accessible);
    saveAdventureAction(stream, onUse);
}

EInvalidConnectionRoom::EInvalidConnectionRoom(const RoomConnection & connection, const Room & room)
    : Exception("Connection \"" + connection.getName() + "\" does not connect the room \"" + room.getName() + "\"")
{
}
