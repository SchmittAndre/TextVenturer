#pragma once

class Inventory;
class Room;
class Location;
class AdventureObject;
class CommandSystem;

class Player
{
private:
    // order specific for load-constructor intializer-list
    std::string name;
    Room & room;
    ref_optional<Location> location;

    CommandSystem & commandSystem;
    Inventory inventory;
    
    std::unordered_set<std::reference_wrapper<AdventureObject>> knownSubjects;

public:
    Player(FileStream & stream, CommandSystem & commandSystem, const ref_vector<AdventureObject> & objectList);
    Player(std::string name, Room & startroom, CommandSystem & commandSystem);
    virtual ~Player();

    void gotoLocation(Location & location);
    void leaveLocation();

    void gotoRoom(Room & room);

    Inventory & getInventory();
    Room & currentRoom();
    Location & currentLocation();
    bool isAtLocation() const;

    bool knows(const AdventureObject & subject) const;
    void inform(AdventureObject & subject);
    void forget(AdventureObject & subject);

    std::string getName() const;
    void rename(std::string name); 

    CommandSystem & getCommandSystem();

    void save(FileStream & stream, ref_idlist<AdventureObject> objectIDs) const;
};

