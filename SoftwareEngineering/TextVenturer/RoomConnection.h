#pragma once

#include "Location.h"

class Room;
class CustomAdventureAction;

class RoomConnection : public Location
{
private:
    Room* room1;
    Room* room2;

    bool accessible;     

    CustomAdventureAction* onUse;

public:                 
    RoomConnection();
    ~RoomConnection();

    void setConnection(Room* room1, Room* room2, bool accessible);

    Room* getOtherRoom(const Room* room) const;
    bool isAccessible() const;

    void lock();
    void unlock();

    CustomAdventureAction* getOnUse();
    void setOnUse(CustomAdventureAction* onUse);

    Type getType();
    void save(FileStream & stream, idlist<AdventureObject*> & objectIDs, idlist<CommandArray*> & commandArrayIDs);
};

