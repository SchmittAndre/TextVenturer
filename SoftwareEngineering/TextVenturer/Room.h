#pragma once

#include "AdventureObject.h"

class Location;
class Player;
class RoomConnection;
class CommandArray;
class CustomAdventureAction;

class Room : public AdventureObject
{
private:
    ref_vector<Location> locations;
    CommandArray locatedCommands;

    CustomAdventureAction * onEnter;
    CustomAdventureAction * onLeave;

public:      
    Room();
    ~Room();

    void addLocation(Location & location);
    void delLocation(Location & location);

    Location & findLocation(std::string name) const;
    RoomConnection & findRoomConnectionTo(std::string name) const;
    Room & findRoom(std::string name) const;

    CommandArray & getLocatedCommands() const;

    CustomAdventureAction * getOnEnter() const;
    CustomAdventureAction * getOnLeave() const;

    void setOnEnter(CustomAdventureAction * onEnter);
    void setOnLeave(CustomAdventureAction * onLeave);

    std::string formatLocations(Player * player) const;

    bool hasLocation(Location* location) const;

    Type getType() const;
    void save(FileStream & stream, idlist<AdventureObject*> & objectIDs, idlist<CommandArray*> & commandArrayIDs) const;
    void load(FileStream & stream, Adventure * adventure, std::vector<AdventureObject*> & objectList, std::vector<CommandArray*> & commandArrayList);
};

class ELocationNotFound : public Exception
{
public:
    ELocationNotFound(const Room & room, const std::string & location);
};

class ELocationDoesNotExist : public Exception
{
public:
    ELocationDoesNotExist(const Room & room, const Location & location);
};

class ELocationExistsAlready : public Exception
{
public:
    ELocationExistsAlready(const Room & room, const Location & location);
};
