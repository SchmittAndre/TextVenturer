#include "stdafx.h"
#include "Inventory.h"
#include "Room.h"
#include "Location.h"

#include "Player.h"

Player::Player(string name, Room* startroom)
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
}

void Player::gotoRoom(Room * room)
{
    this->room = room;
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

string Player::getName() const
{
    return name;
}

void Player::rename(string name)
{
    this->name = name;
}
