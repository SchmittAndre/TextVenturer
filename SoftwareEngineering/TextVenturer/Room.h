#pragma once

class Location;

class Room
{
private:
    string name;
    string description;

    vector<Location*> locations;

public:
    Room(string name, string description);

    bool addLocation(Location* location);
    bool delLocation(Location* location);

};

