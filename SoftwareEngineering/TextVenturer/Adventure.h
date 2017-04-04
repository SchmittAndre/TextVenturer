#pragma once

class CmdLine;
class CommandSystem;
class Player;
class DefaultAdventureAction;
class CustomAdventureAction;
class AdventureAction;
class Command;
class AdventureObject;
class ItemCombiner;

class Adventure
{
private:

    // "constant"

    CmdLine* cmdLine;

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
	AdventureAction* exitAction;
    
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
	Command* exitCommand;

    bool initialized; // loaded without error

    // state specific (must get saved)

    std::string title;
    std::string description;

    std::unordered_map<std::string, AdventureObject*> objects;
    
    CommandSystem* commandSystem;
    Player* player;
    CustomAdventureAction* onInit;
    
    ItemCombiner* itemCombiner;

    tags globalFlags;

    bool running;     
                          
    void initDefaultActions();

public:
    Adventure();
    virtual ~Adventure();

    bool loadFromFile(std::wstring filename);
    
    bool loadState(std::wstring filename);
    bool saveState(std::wstring filename);

    void sendCommand(std::string command) const;

    Player* getPlayer() const;
    ItemCombiner* getItemCombiner() const;

    CmdLine* getCmdLine() const;

    AdventureObject* findObjectByAlias(std::string alias) const;
    AdventureObject* findObjectByName(std::string name) const;

    void setFlag(std::string flag);
    void clearFlag(std::string flag);
    bool testFlag(std::string flag);      
    
    void start(CmdLine* cmdLine);

    bool isInitialized() const;
    bool isRunning() const;
    void update() const;
};

