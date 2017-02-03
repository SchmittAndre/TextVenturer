#include "stdafx.h"

#include "Player.h"

Player::Player(std::string name, Room* startroom)
{
    this->name = name;
    room = startroom;
    inventory = new Inventory();
}

Player::~Player()
{
    delete inventory;
}

void Player::gotoLocation(Location * location)
{
    this->location = location;
    inform(location);
}

void Player::gotoRoom(Room * room)
{
    this->room = room;
    inform(room);
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

bool Player::knows(void * subject) const
{
    return knownSubjects.find(subject) != knownSubjects.end();
}

void Player::inform(void * subject)
{
    knownSubjects.insert(subject);
}

void Player::forget(void * subject)
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
