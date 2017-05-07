#include "stdafx.h"

#include "Room.h"
#include "Inventory.h"
#include "Location.h"
#include "CommandSystem.h"
#include "AdventureObject.h"
#include "RoomConnection.h"

#include "Player.h"

Player::Player(FileStream & stream, CommandSystem & commandSystem, const ref_vector<AdventureObject> & objectList)
    : name(stream.readString())
    , commandSystem(commandSystem)
    , room(dynamic_cast<Room&>(objectList[stream.readUInt()].get()))
    , inventory(stream, objectList, *this)
{
    commandSystem.add(room.getLocatedCommands());
    if (stream.readBool())
        location = static_cast<Location&>(objectList[stream.readUInt()].get());
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        knownSubjects.insert(objectList[stream.readUInt()]);
}

Player::Player(std::string name, Room & startRoom, CommandSystem & commandSystem)
    : name(name)
    , room(startRoom)
    , commandSystem(commandSystem)
    , inventory(*this)
{
    commandSystem.add(room.getLocatedCommands());
}

Player::~Player()
{
    commandSystem.del(room.getLocatedCommands());
}

void Player::gotoLocation(Location & location)
{
    if (this->location && &this->location->get() == &location)
        return;
    if (this->location)
    {
        commandSystem.del(this->location->get().getLocatedCommands());
    }
    this->location = location;
    commandSystem.add(location.getLocatedCommands());
}

void Player::leaveLocation()
{                    
    if (location)
        commandSystem.del(location->get().getLocatedCommands());
    location.reset();
}

void Player::gotoRoom(Room & room)
{
    if (&this->room == &room)
        return;
    commandSystem.del(this->room.getLocatedCommands());
    this->room = room;
    commandSystem.add(room.getLocatedCommands());
    leaveLocation();
}

Inventory & Player::getInventory() 
{
    return inventory;
}

Room & Player::currentRoom() 
{
    return room;
}

Location & Player::currentLocation()
{
    return location.value().get();
}

bool Player::isAtLocation() const
{
    return location.has_value();
}

bool Player::knows(const AdventureObject & subject) const
{
    return std::find(knownSubjects.cbegin(), knownSubjects.cend(), subject) != knownSubjects.cend();
}

void Player::inform(AdventureObject & subject)
{
    knownSubjects.insert(subject);
}

void Player::forget(AdventureObject & subject)
{
    knownSubjects.erase(subject);
}
        
std::string Player::getName() const
{
    return name;
}

void Player::rename(std::string name)
{
    this->name = name;
}

CommandSystem & Player::getCommandSystem() 
{
    return commandSystem;
}

void Player::save(FileStream & stream, ref_idlist<AdventureObject> objectIDs) const
{
    stream.write(name);
    stream.write(objectIDs[room]);
    stream.write(location.has_value());
    if (location)
        stream.write(objectIDs[location.value().get()]);
    inventory.save(stream, objectIDs);
    stream.write(static_cast<UINT>(knownSubjects.size()));
    for (AdventureObject & subject : knownSubjects)
        stream.write(objectIDs[subject]);
}
