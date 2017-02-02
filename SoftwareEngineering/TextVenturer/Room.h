#pragma once

#include "RoomConnection.h"

class RoomConnection;
class Location;

class Room
{
private:
    AliasList* aliases;
    string description;

    vector<Location*> locations;

public:
    Room(string name, string description);
    virtual ~Room();

    bool addLocation(Location* location);
    bool delLocation(Location* location);

    vector<Location*> getLocations() const;
    Location* findLocation(string name) const;
    RoomConnection* findRoomConnectionTo(string name) const;
    Room* findRoom(string name) const;

    AliasList* getAliases() const;
    string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    string getName(Player* player, bool startOfSentence = false) const;
    string getDescription() const;

    string formatLocations(Player* player) const;
};

