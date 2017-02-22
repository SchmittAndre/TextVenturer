#pragma once

#include "AdventureObject.h"

class Location;
class Player;
class RoomConnection;
class CommandArray;

class Room : public AdventureObject
{
private:
    std::vector<Location*> locations;
    CommandArray* locatedCommands;

public:      
    Room();
    ~Room();

    bool addLocation(Location* location);
    bool delLocation(Location* location);

    std::vector<Location*> getLocations() const;
    Location* findLocation(std::string name) const;
    RoomConnection* findRoomConnectionTo(std::string name) const;
    Room* findRoom(std::string name) const;

    CommandArray* getLocatedCommands();

    std::string formatLocations(Player* player) const;
};

