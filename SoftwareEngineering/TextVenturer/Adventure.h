#pragma once

#include "Controler.h"
#include "Player.h"
#include "ItemCombiner.h"
#include "DefaultAdventureAction.h"

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
    AdventureAction* pickupFromAction;
    AdventureAction* pickupAction;
    AdventureAction* useRoomConnectionAction; 
    AdventureAction* gotoAction; 
    AdventureAction* enterRoomAction;
    AdventureAction* combineItemsAction; 
    
    Command* helpCommand;
    Command* showInventoryCommand;
    Command* lookAroundCommand;
    Command* inspectCommand;
    Command* pickupFromCommand;
    Command* pickupCommand;
    Command* useRoomConnectionCommand;
    Command* gotoCommand;
    Command* enterRoomCommand;
    Command* combineItemsCommand;
    
    std::unordered_map<std::string, Room*> rooms;
    std::unordered_map<std::string, Location*> locations;
    std::unordered_map<std::string, Item*> items;

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

    bool isInitialized() const;

    void update() const;
};

