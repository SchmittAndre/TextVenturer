#include "stdafx.h"

#include "AliasList.h"
#include "Location.h"
#include "RoomConnection.h"
#include "Player.h"          
#include "CommandSystem.h"
#include "CustomAdventureAction.h"

#include "Room.h"

AdventureObject::Type Room::getType() const
{
    return otRoom;
}

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
    throw(ELocationNotFound, this, name);
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

CommandArray * Room::getLocatedCommands() const
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

bool Room::hasLocation(Location * location) const
{
    return find(locations.cbegin(), locations.cend(), location) != locations.cend();
}

void Room::save(FileStream & stream, idlist<AdventureObject*>& objectIDs, idlist<CommandArray*>& commandArrayIDs) const
{
    AdventureObject::save(stream, objectIDs, commandArrayIDs);
    stream.write(static_cast<UINT>(locations.size()));
    for (Location* location : locations)
        stream.write(objectIDs[location]);
    locatedCommands->save(stream);
    commandArrayIDs[locatedCommands] = static_cast<UINT>(commandArrayIDs.size());
    saveAdventureAction(stream, onEnter);
    saveAdventureAction(stream, onLeave);
}

void Room::load(FileStream & stream, Adventure * adventure, std::vector<AdventureObject*>& objectList, std::vector<CommandArray*>& commandArrayList)
{
    AdventureObject::load(stream, adventure, objectList, commandArrayList);
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        locations.push_back(static_cast<Location*>(objectList[stream.readUInt()]));
    locatedCommands->load(stream, adventure);
    commandArrayList.push_back(locatedCommands);
    loadAdventureAction(stream, adventure, onEnter);
    loadAdventureAction(stream, adventure, onLeave);
}

ELocationNotFound::ELocationNotFound(const Room * room, std::string location)
    : Exception("Could not find location \"" + location + "\" in room \"" + room->getName() + "\"")
{
}
