#pragma once

class Contoler;
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
    AdventureAction* pickupAction; 
    AdventureAction* useRoomConnectionAction; 
    AdventureAction* gotoAction; 
    AdventureAction* enterRoomAction;
    AdventureAction* combineItemsAction; 
    AdventureAction* useItemAction; 

    AdventureAction* lockLocationAction;
    AdventureAction* unlockLocationAction;

    Command* helpCommand;
    Command* showInventoryCommand;
    Command* lookAroundCommand;
    Command* inspectCommand;
    Command* pickupCommand;
    Command* useRoomConnectionCommand;
    Command* gotoCommand;
    Command* enterRoomCommand;
    Command* combineItemsCommand;
    Command* useItemCommand;

    vector<Room*> rooms;
    vector<Location*> locations;
    vector<Item*> items;

    ItemCombiner* itemCombiner;

    bool initialized;
    
public:
    Adventure(Controler* controler, string filename = "DEBUG");
    virtual ~Adventure();

    void loadFromFile(string filename);
    void DEBUG_loadTest();

    void sendCommand(string command) const;

    Player* getPlayer() const;
    ItemCombiner* getItemCombiner() const;

    Controler* getControler() const;

    bool isInitialized() const;
};

