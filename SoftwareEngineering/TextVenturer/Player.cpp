#include "stdafx.h"

#include "Room.h"
#include "Inventory.h"
#include "Location.h"
#include "CommandSystem.h"
#include "AdventureObject.h"
#include "RoomConnection.h"

#include "Player.h"

Player::Player(std::string name, Room* startRoom, CommandSystem* commandSystem)
{
    this->name = name;         
    room = startRoom;
    this->commandSystem = commandSystem;
    if (startRoom)
    commandSystem->add(startRoom->getLocatedCommands());
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
    this->location = location;
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
    location = NULL;
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
