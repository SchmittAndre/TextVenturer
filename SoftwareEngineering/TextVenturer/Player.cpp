#include "stdafx.h"
#include "Inventory.h"
#include "Room.h"
#include "Location.h"

#include "Player.h"

Player::Player(string name)
{
    this->name = name;
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

Inventory* Player::getInventory()
{
    return inventory;
}

Room * Player::currentRoom()
{
    return room;
}

Location * Player::currentLocation()
{
    return location;
}

bool Player::isAtLocation()
{
    return location != NULL;
}

string Player::getName()
{
    return string();
}

void Player::rename(string name)
{
}
