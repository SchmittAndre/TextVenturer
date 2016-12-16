#pragma once

class AliasList;
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
    ~Room();

    bool addLocation(Location* location);
    bool delLocation(Location* location);

    Location* findLocation(string name);

    AliasList* getAliases();
    string getDescription();     
};

