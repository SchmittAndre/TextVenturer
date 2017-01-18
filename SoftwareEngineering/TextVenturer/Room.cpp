#include "stdafx.h"
#include "Location.h"
#include "RoomConnection.h"
#include "Player.h"

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
    if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        connection->getOtherRoom(this)->delLocation(connection);
    return true;
}

vector<Location*> Room::getLocations() const
{
    return locations;
}

Location* Room::findLocation(string name) const
{
    for (Location* location : locations)
        if (location->getAliases()->has(name))
            return location;
    return NULL;
}

RoomConnection * Room::findRoomConnectionTo(string name) const
{
    for (Location* location : locations)
        if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        {
            if (connection->isAccessible())
            {
                Room* room = connection->getOtherRoom(this);
                if (room->getAliases()->has(name))
                    return connection;
            }
        }
    return NULL;
}

Room * Room::findRoom(string name) const
{
    if (RoomConnection* connection = findRoomConnectionTo(name))
        return connection->getOtherRoom(this);
    return NULL;
}

AliasList* Room::getAliases() const
{
    return aliases;
}

string Room::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases->getName(definiteArticle, startOfSentence);
}

string Room::getName(Player * player, bool startOfSentence) const
{
    return getName(player->knows((Room*)this), startOfSentence);
}

string Room::getDescription() const
{
    return description;
}

string Room::formatLocations(Player* player) const
{
    if (locations.empty())
        return "nothing";
    string result = "";
    for (vector<Location*>::const_iterator location = locations.begin(); location != locations.end() - 1; location++)
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
