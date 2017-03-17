#pragma once

class Controler;
class CommandSystem;
class Player;
class DefaultAdventureAction;
class AdventureAction;
class Command;
class AdventureObject;
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

    tags globalFlags;

    bool initialized;

public:
    Adventure(Controler* controler);
    virtual ~Adventure();

    bool loadFromFile(std::string filename);
    
    bool loadState(std::string filename);
    bool saveState(std::string filename);

    void sendCommand(std::string command) const;

    Player* getPlayer() const;
    ItemCombiner* getItemCombiner() const;

    Controler* getControler() const;

    AdventureObject* findObjectByAlias(std::string alias) const;
    AdventureObject* findObjectByName(std::string name) const;

    void setFlag(std::string flag);
    void clearFlag(std::string flag);
    bool testFlag(std::string flag);

    
    bool isInitialized() const;

    void update() const;
};

