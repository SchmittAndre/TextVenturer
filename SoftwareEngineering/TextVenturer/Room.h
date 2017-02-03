#pragma once

#include "RoomConnection.h"

class RoomConnection;
class Location;

class Room
{
private:
    AliasList* aliases;
    std::string description;

    std::vector<Location*> locations;

public:
    Room(std::string name, std::string description);
    virtual ~Room();

    bool addLocation(Location* location);
    bool delLocation(Location* location);

    std::vector<Location*> getLocations() const;
    Location* findLocation(std::string name) const;
    RoomConnection* findRoomConnectionTo(std::string name) const;
    Room* findRoom(std::string name) const;

    AliasList* getAliases() const;
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player* player, bool startOfSentence = false) const;
    std::string getDescription() const;

    std::string formatLocations(Player* player) const;
};

