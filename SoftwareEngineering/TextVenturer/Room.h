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
    std::vector<Location*> locations;
    CommandArray* locatedCommands;

    CustomAdventureAction* onEnter;
    CustomAdventureAction* onLeave;

public:      
    Room();
    ~Room();

    bool addLocation(Location* location);
    bool delLocation(Location* location);

    std::vector<Location*> getLocations() const;
    Location* findLocation(std::string name) const;
    RoomConnection* findRoomConnectionTo(std::string name) const;
    Room* findRoom(std::string name) const;

    CommandArray* getLocatedCommands() const;

    CustomAdventureAction* getOnEnter() const;
    CustomAdventureAction* getOnLeave() const;

    void setOnEnter(CustomAdventureAction* onEnter);
    void setOnLeave(CustomAdventureAction* onLeave);

    std::string formatLocations(Player* player) const;

    bool hasLocation(Location* location) const;

    Type getType() const;
    void save(FileStream & stream, idlist<AdventureObject*> & objectIDs, idlist<CommandArray*> & commandArrayIDs) const;
    void load(FileStream & stream, Adventure * adventure, std::vector<AdventureObject*> & objectList, std::vector<CommandArray*> & commandArrayList);
};

class ELocationNotFound : public Exception
{
public:
    ELocationNotFound(const Room* room, std::string location);
};
