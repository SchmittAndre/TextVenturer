#include "stdafx.h"

#include "Room.h"
#include "Location.h"
#include "CommandSystem.h"
#include "AdventureObject.h"
#include "RoomConnection.h"
#include "Item.h"
#include "Adventure.h"

#include "Player.h"

Location * Player::loadLocation(FileStream & stream, AdventureLoadHelp & help)
{
    if (stream.readBool())
        return &dynamic_cast<Location&>(help.objects[stream.readUInt()].get());
    return NULL;
}

Player::Player(FileStream & stream, CommandSystem & commandSystem, AdventureLoadHelp & help)
    : name(stream.readString())
    , room(&dynamic_cast<Room&>(help.objects[stream.readUInt()].get()))
    , location(loadLocation(stream, help))
    , inventory(stream, help, *this)
    , commandSystem(commandSystem)
{
    commandSystem.add(room->getLocatedCommands());
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        knownSubjects.insert(&help.objects[stream.readUInt()].get());
}

Player::Player(std::string name, Room & startRoom, CommandSystem & commandSystem)
    : name(name)
    , room(&startRoom)
    , commandSystem(commandSystem)
    , inventory(*this)
{
    commandSystem.add(room->getLocatedCommands());
}

Player::~Player()
{
    commandSystem.del(room->getLocatedCommands());
}

void Player::gotoLocation(Location & location)
{
    if (this->location && this->location == &location)
        return;
    if (this->location)
    {
        commandSystem.del(this->location->getLocatedCommands());
    }
    this->location = &location;
    commandSystem.add(location.getLocatedCommands());
}

void Player::leaveLocation()
{                    
    if (location)
        commandSystem.del(location->getLocatedCommands());
    location = NULL;
}

void Player::gotoRoom(Room & room)
{
    if (this->room == &room)
        return;
    commandSystem.del(this->room->getLocatedCommands());
    this->room = &room;
    commandSystem.add(room.getLocatedCommands());
    leaveLocation();
}

Inventory & Player::getInventory() 
{
    return inventory;
}

Room & Player::currentRoom() 
{
    return *room;
}

Location & Player::currentLocation()
{
    if (!location)
        throw(ENotAtLocation);
    return *location;
}

bool Player::isAtLocation() const
{
    return location;
}

bool Player::knows(const AdventureObject & subject) const
{
    return std::find(knownSubjects.cbegin(), knownSubjects.cend(), &subject) != knownSubjects.cend();
}

void Player::inform(AdventureObject & subject)
{
    knownSubjects.insert(&subject);
}

void Player::forget(AdventureObject & subject)
{
    knownSubjects.erase(&subject);
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

void Player::save(FileStream & stream, AdventureSaveHelp & help) const
{
    stream.write(name);
    stream.write(help.objects[room]);
    stream.write(location != NULL);
    if (location)
        stream.write(help.objects[location]);
    inventory.save(stream, help);
    stream.write(static_cast<UINT>(knownSubjects.size()));
    for (AdventureObject * subject : knownSubjects)
        stream.write(help.objects[subject]);
}

Player::PlayerInventory::PlayerInventory(FileStream & stream, AdventureLoadHelp & help, Player & player)
    : Inventory(stream, help)
    , player(player)
{
}

Player::PlayerInventory::PlayerInventory(Player & player)
    : Inventory()    
    , player(player)
{
}

void Player::PlayerInventory::addItem(Item & item)
{
    Inventory::addItem(item);
    player.getCommandSystem().add(item.getCarryCommands());    
}

void Player::PlayerInventory::delItem(Item & item)
{
    Inventory::delItem(item);
    player.getCommandSystem().del(item.getCarryCommands());
}

ENotAtLocation::ENotAtLocation()
    : Exception("The player is not standing at a location")
{
}
