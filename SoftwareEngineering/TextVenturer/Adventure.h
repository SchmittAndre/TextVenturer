#pragma once

class Controler;
class CommandSystem;
class Player;
class DefaultAdventureAction;
class AdventureAction;
class Command;
class Room;
class Location;
class Item;
class ItemCombiner;

class Adventure
{
private:
    Controler* controler;

    CommandSystem* commandSystem;          
    Player* player;

    DefaultAdventureAction* defaultAction;

    AdventureAction* helpAction;
    AdventureAction* showInventoryAction; 
    AdventureAction* lookAroundAction;
    AdventureAction* inspectAction; 
    AdventureAction* takeFromAction;
    AdventureAction* takeAction;
    AdventureAction* placeAction;
    AdventureAction* useRoomConnectionAction; 
    AdventureAction* gotoAction; 
    AdventureAction* enterRoomAction;
    AdventureAction* combineItemsAction; 
    
    Command* helpCommand;
    Command* showInventoryCommand;
    Command* lookAroundCommand;
    Command* inspectCommand;
    Command* takeFromCommand;
    Command* takeCommand;
    Command* placeCommand;
    Command* useRoomConnectionCommand;
    Command* gotoCommand;
    Command* enterRoomCommand;
    Command* combineItemsCommand;
    
    std::unordered_map<std::string, AdventureObject*> objects;

    ItemCombiner* itemCombiner;

    std::string title;
    std::string description;

    bool initialized;

public:
    Adventure(Controler* controler);
    virtual ~Adventure();

    bool loadFromFile(std::string filename);
    void DEBUG_loadTest();

    void sendCommand(std::string command) const;

    Player* getPlayer() const;
    ItemCombiner* getItemCombiner() const;

    Controler* getControler() const;

    AdventureObject* findObjectByAlias(std::string alias) const;
    AdventureObject* findObjectByName(std::string name) const;

    bool isInitialized() const;

    void update() const;
};

