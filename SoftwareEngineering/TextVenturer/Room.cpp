#include "stdafx.h"
#include "Location.h"

#include "Room.h"

Room::Room(string name, string description)
{
    this->name = name;
    this->description = description;
}

bool Room::addLocation(Location* location)
{
    if (find(locations.begin(), locations.end(), location) != locations.end())
        return false;
    locations.push_back(location);
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
