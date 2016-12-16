#include "stdafx.h"
#include "Location.h"
#include "RoomConnection.h"

#include "Room.h"

Room::Room(string name, string description)
{
    aliases = new AliasList(name);
    this->description = description;
}

Room::~Room()
{
    delete aliases;
}

bool Room::addLocation(Location* location)
{
    if (find(locations.begin(), locations.end(), location) != locations.end())
        return false;
    locations.push_back(location);
    if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        connection->getOtherRoom(this)->addLocation(connection);                 
    return true;
}

bool Room::delLocation(Location* location)
{
    vector<Location*>::iterator i = find(locations.begin(), locations.end(), location);
    if (i == locations.end())
        return false;
    locations.erase(i);
    return true;
}

Location* Room::findLocation(string name)
{
    for (Location* location : locations)
        if (location->getAliases()->has(name))
            return location;
    return NULL;
}

AliasList* Room::getAliases()
{
    return aliases;
}

string Room::getDescription()
{
    return description;
}
