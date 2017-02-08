#pragma once

#include "AdventureObject.h"

class Location;
class Player;
class RoomConnection;

class Room : public AdventureObject
{
private:
    std::vector<Location*> locations;

public:      
    bool addLocation(Location* location);
    bool delLocation(Location* location);

    std::vector<Location*> getLocations() const;
    Location* findLocation(std::string name) const;
    RoomConnection* findRoomConnectionTo(std::string name) const;
    Room* findRoom(std::string name) const;

    std::string formatLocations(Player* player) const;
};

