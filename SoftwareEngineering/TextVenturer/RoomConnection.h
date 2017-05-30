#pragma once

#include "Location.h"

class Room;

class RoomConnection : public Location
{
private:
    Room & room1;
    Room & room2;

    bool accessible;     

    CustomAdventureAction * onUse;

public:                 
    RoomConnection(Room & room1, Room & room2, bool accessible);
    RoomConnection(FileStream & stream, AdventureLoadHelp & help);
    ~RoomConnection();

    Room & getOtherRoom(const Room & room) const;
    bool isAccessible() const;

    void lock();
    void unlock();

    CustomAdventureAction* getOnUse() const;
    void setOnUse(CustomAdventureAction* onUse);

    Type getType() const;
    void save(FileStream & stream, AdventureSaveHelp & help) const;
};

class EInvalidConnectionRoom : public Exception
{
public:
    EInvalidConnectionRoom(const RoomConnection & connection, const Room & room);
};