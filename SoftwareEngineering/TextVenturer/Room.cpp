#include "stdafx.h"

#include "Location.h"
#include "RoomConnection.h"
#include "Player.h"          
#include "CommandSystem.h"
#include "CustomAdventureAction.h"

#include "Room.h"

Room::Room()
{
    locatedCommands = new CommandArray();
    onEnter = NULL;
    onLeave = NULL;
}

Room::~Room()
{
    delete locatedCommands;
    delete onEnter;
    delete onLeave;
}

bool Room::addLocation(Location* location)
{
    if (find(locations.begin(), locations.end(), location) != locations.end())
        return false;
    locations.push_back(location);
    if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        connection->getOtherRoom(this)->addLocation(connection);        
    location->room = this;
    return true;
}

bool Room::delLocation(Location* location)
{
    std::vector<Location*>::iterator i = find(locations.begin(), locations.end(), location);
    if (i == locations.end())
        return false;
    locations.erase(i);
    if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        connection->getOtherRoom(this)->delLocation(connection);
    return true;
}

std::vector<Location*> Room::getLocations() const
{
    return locations;
}

Location* Room::findLocation(std::string name) const
{
    for (Location* location : locations)
        if (location->getAliases().has(name))
            return location;
    return NULL;
}

RoomConnection * Room::findRoomConnectionTo(std::string name) const
{
    for (Location* location : locations)
        if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        {
            if (connection->isAccessible())
            {
                Room* room = connection->getOtherRoom(this);
                if (room->getAliases().has(name))
                    return connection;
            }
        }
    return NULL;
}

Room * Room::findRoom(std::string name) const
{
    if (RoomConnection* connection = findRoomConnectionTo(name))
        return connection->getOtherRoom(this);
    return NULL;
}

CommandArray * Room::getLocatedCommands()
{
    return locatedCommands;
}

void Room::runOnEnter()
{
    if (onEnter)
        onEnter->run();
}

void Room::runOnLeave()
{
    if (onLeave)
        onLeave->run();
}

void Room::setOnEnter(CustomAdventureAction * onEnter)
{
    this->onEnter = onEnter;
}

void Room::setOnLeave(CustomAdventureAction * onLeave)
{
    this->onLeave = onLeave;
}

std::string Room::formatLocations(Player* player) const
{
    if (locations.empty())
        return "nothing";
    std::string result = "";
    for (std::vector<Location*>::const_iterator location = locations.begin(); location != locations.end() - 1; location++)
    {
        result += (*location)->getName(player);
        if (location != locations.end() - 2)
            result += ", ";
    }
    if (result != "")
        result += " and ";
    result += (*(locations.end() - 1))->getName(player);
    return result;
}
