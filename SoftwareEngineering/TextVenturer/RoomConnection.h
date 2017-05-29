#pragma once

#include "Location.h"

class Room;
class CustomAdventureAction;

class RoomConnection : public Location
{
private:
    Room & room1;
    Room & room2;

    bool accessible;     

    CustomAdventureAction* onUse;

public:                 
    RoomConnection(Room & room1, Room & room2, bool accessible);
    RoomConnection(FileStream & stream, Adventure & adventure, ref_vector<AdventureObject> & objectList, ref_vector<CommandArray> & commandArrayList);
    ~RoomConnection();

    Room & getOtherRoom(const Room & room) const;
    bool isAccessible() const;

    void lock();
    void unlock();

    CustomAdventureAction* getOnUse() const;
    void setOnUse(CustomAdventureAction* onUse);

    Type getType() const;
    void save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs, ref_idlist<CommandArray> & commandArrayIDs) const;
};

class EInvalidConnectionRoom : public Exception
{
public:
    EInvalidConnectionRoom(const RoomConnection & connection, const Room & room);
};