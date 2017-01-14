#pragma once

class Location;
class RoomConnection;

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
    Room* findRoom(string name) const;

    AliasList* getAliases() const;
    string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    string getDescription() const;

    string formatLocations() const;
};

