#include "stdafx.h"

#include "Room.h"
#include "Inventory.h"
#include "Location.h"
#include "CommandSystem.h"
#include "AdventureObject.h"
#include "RoomConnection.h"

#include "Player.h"

Player::Player(FileStream & stream, CommandSystem* commandSystem, std::vector<AdventureObject*> objectList)
{
    this->commandSystem = commandSystem;
    stream.read(name);
    room = static_cast<Room*>(objectList[stream.readUInt()]);
    commandSystem->add(room->getLocatedCommands());
    if (stream.readBool())
        location = static_cast<Location*>(objectList[stream.readUInt()]);
    else
        location = NULL;
    inventory = new Inventory(stream, objectList, this);
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        knownSubjects.insert(objectList[stream.readUInt()]);
}

Player::Player(std::string name, Room* startRoom, CommandSystem* commandSystem)
{
    this->name = name;         
    room = startRoom;
    this->commandSystem = commandSystem;
    commandSystem->add(room->getLocatedCommands());
    inventory = new Inventory(this);
}

Player::~Player()
{
    if (room)
        commandSystem->del(room->getLocatedCommands());
    delete inventory;
}

void Player::gotoLocation(Location * location)
{
    if (this->location == location)
        return;
    if (this->location)
    {
        commandSystem->del(this->location->getLocatedCommands());
    }
    this->location = location;
    if (location)
    {
        commandSystem->add(location->getLocatedCommands());
    }
}

void Player::gotoRoom(Room * room)
{
    if (this->room == room)
        return;
    if (this->room)
    {
        commandSystem->del(this->room->getLocatedCommands());
    }
    this->room = room;
    if (room)
    {
        commandSystem->add(room->getLocatedCommands());
    }
    gotoLocation(NULL);
}

Inventory* Player::getInventory() const
{
    return inventory;
}

Room * Player::currentRoom() const
{
    return room;
}

Location * Player::currentLocation() const
{
    return location;
}

bool Player::isAtLocation() const
{
    return location != NULL;
}

bool Player::knows(AdventureObject * subject) const
{
    return knownSubjects.find(subject) != knownSubjects.end();
}

void Player::inform(AdventureObject * subject)
{
    knownSubjects.insert(subject);
}

void Player::forget(AdventureObject * subject)
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

CommandSystem * Player::getCommandSystem() const
{
    return commandSystem;
}

void Player::save(FileStream & stream, idlist<AdventureObject*> objectIDs) const
{
    stream.write(name);
    stream.write(objectIDs[room]);
    stream.write(location != NULL);
    if (location)
        stream.write(objectIDs[location]);
    inventory->save(stream, objectIDs);
    stream.write(static_cast<UINT>(knownSubjects.size()));
    for (AdventureObject* subject : knownSubjects)
        stream.write(objectIDs[subject]);
}
