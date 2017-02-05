#pragma once

#include "RoomConnection.h"

class RoomConnection;
class Location;

class Room
{
private:
    AliasList aliases;
    std::string description = "Missing description!";

    std::vector<Location*> locations;

public:      
    bool addLocation(Location* location);
    bool delLocation(Location* location);

    std::vector<Location*> getLocations() const;
    Location* findLocation(std::string name) const;
    RoomConnection* findRoomConnectionTo(std::string name) const;
    Room* findRoom(std::string name) const;

    AliasList& getAliases();
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player* player, bool startOfSentence = false) const;
    void setDescription(std::string description);
    std::string getDescription() const;

    std::string formatLocations(Player* player) const;
};

