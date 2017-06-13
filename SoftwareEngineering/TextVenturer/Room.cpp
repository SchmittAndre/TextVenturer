#include "stdafx.h"

#include "AliasList.h"
#include "Location.h"
#include "RoomConnection.h"
#include "Player.h"          
#include "CommandSystem.h"
#include "CustomAdventureAction.h"
#include "Adventure.h"

#include "Room.h"

AdventureObject::Type Room::getType() const
{
    return otRoom;
}

Room::Room()
    : onEnter(NULL)
    , onLeave(NULL)
{
}

Room::Room(FileStream & stream, AdventureLoadHelp & help)
    : AdventureObject(stream, help)
    , locatedCommands(stream, help)
    , onEnter(CustomAdventureAction::loadConditional(stream, help.adventure))
    , onLeave(CustomAdventureAction::loadConditional(stream, help.adventure))
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        locations.push_back(dynamic_cast<Location&>(help.objects[stream.readUInt()].get()));
    help.commandArrays.push_back(locatedCommands);
}

Room::~Room()
{
    delete onEnter;
    delete onLeave;
}

void Room::addLocation(Location & location)
{
    if (std::find_if(locations.begin(), locations.end(), [&](Location & a)
    {
        return &a == &location;
    }) != locations.end())
        throw(ELocationExistsAlready, *this, location);
    locations.push_back(location);
}

void Room::delLocation(Location & location)
{
    auto pos = std::find_if(locations.begin(), locations.end(), [&](Location & a)
    {
        return &a == &location;
    });
    if (pos == locations.end())
        throw(ELocationDoesNotExist, *this, location);
    locations.erase(pos);
    if (RoomConnection * connection = dynamic_cast<RoomConnection*>(&location))
    {
        connection->getOtherRoom(*this).delLocation(*connection);
    }
}

Location & Room::findLocation(std::string name) const
{
    for (Location & location : locations)
        if (location.getAliases().has(name))
            return location;
    throw(ELocationNotFound, *this, name);
}

RoomConnection & Room::findRoomConnectionTo(std::string name)
{
    for (Location & location : locations)
    {
        if (RoomConnection * connection = dynamic_cast<RoomConnection*>(&location))
        {
            if (connection->isAccessible())
            {
                if (connection->getOtherRoom(*this).getAliases().has(name))
                    return *connection;
            }
        }
    }
    // no return happened
    throw(ERoomNotFound, *this, name);
}

Room & Room::findRoom(std::string name)
{
    RoomConnection & connection = findRoomConnectionTo(name);
    return connection.getOtherRoom(*this);
}

const ref_vector<Location> & Room::getLocations() const
{
    return locations;
}

CommandArray & Room::getLocatedCommands()
{
    return locatedCommands;
}

CustomAdventureAction* Room::getOnEnter() const
{                               
    return onEnter;
}

CustomAdventureAction* Room::getOnLeave() const
{
    return onLeave;
}

void Room::setOnEnter(CustomAdventureAction * onEnter)
{
    this->onEnter = onEnter;
}

void Room::setOnLeave(CustomAdventureAction * onLeave)
{
    this->onLeave = onLeave;
}

std::string Room::formatLocations(Player & player) const
{
    if (locations.empty())
        return "nothing";
    std::string result = "";
    for (auto location = locations.begin(); location != locations.end() - 1; location++)
    {
        result += location->get().getName(player);
        if (location != locations.end() - 2)
            result += ", ";
    }
    if (result != "")
        result += " and ";
    result += locations.back().get().getName(player);
    return result;
}

bool Room::hasLocation(const Location & location) const
{
    return std::find_if(locations.begin(), locations.end(), [&](Location & a) {
        return &a == &location;
    }) != locations.end();
}

void Room::save(FileStream & stream, AdventureSaveHelp & help) const
{
    AdventureObject::save(stream, help);
    locatedCommands.save(stream, help);
    help.commandArrays[&locatedCommands] = static_cast<UINT>(help.commandArrays.size());
    CustomAdventureAction::saveConditional(stream, onEnter);
    CustomAdventureAction::saveConditional(stream, onLeave);
    UINT count = 0;
    for (Location & location : locations)
        if (!dynamic_cast<RoomConnection*>(&location))
            count++;
    stream.write(count);
    for (Location & location : locations)
        if(!dynamic_cast<RoomConnection*>(&location))
            stream.write(help.objects[&location]);
}

ELocationNotFound::ELocationNotFound(const Room & room, const std::string & location)
    : Exception("Could not find location \"" + location + "\" in room \"" + room.getNameOnly() + "\"")
{
}

ELocationDoesNotExist::ELocationDoesNotExist(const Room & room, const Location & location)
    : Exception("Room \"" + room.getNameOnly() + "\" does not have location \"" + location.getNameOnly() + "\"")
{
}

ELocationExistsAlready::ELocationExistsAlready(const Room & room, const Location & location)
    : Exception("Room \"" + room.getNameOnly() + "\" already has location \"" + location.getNameOnly() + "\"")
{
}

ERoomNotFound::ERoomNotFound(const Room & room, const std::string & alias)
    : Exception("Room \"" + room.getNameOnly() + "\" is not connected to \"" + alias + "\"")
{
}
