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

    ref_optional<CmdLine> cmdLine;

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
    
    Command helpCommand;
    Command showInventoryCommand;
    Command lookAroundCommand;
    Command inspectCommand;
    Command takeFromCommand;
    Command takeCommand;
    Command placeCommand;
    Command useRoomConnectionCommand;
    Command gotoCommand;
    Command enterRoomCommand;
    Command combineItemsCommand;
	Command exitCommand;

    bool initialized; // loaded without error

    // state specific (must get saved)

    std::string title;
    std::string description;

    std::unordered_map<std::string, AdventureObject> objects;
    
    CommandSystem commandSystem;
    Player* player;
    CustomAdventureAction* onInit;
    
    ItemCombiner itemCombiner;

    taglist globalFlags;

    bool running;     
                          
    void initDefaultActions();

public:
    Adventure();
    Adventure(std::wstring filename);
    virtual ~Adventure();

    bool saveState(std::wstring filename) const;

    void sendCommand(std::string command);

    Player & getPlayer();
    ItemCombiner & getItemCombiner();

    CmdLine & getCmdLine();

    AdventureObject& findObjectByAlias(std::string alias) const;
    AdventureObject& findObjectByName(std::string name) const;

    bool objectExists(std::string name) const;

    void setFlag(std::string flag);
    void clearFlag(std::string flag);
    void toggleFlag(std::string flag);
    bool testFlag(std::string flag) const;
    
    void start(CmdLine* cmdLine);

    bool isInitialized() const;
    bool isRunning() const;
    void update();

    std::string getTitle() const;
    std::string getDescription() const;
};

class EAdventure : public Exception
{
public:
    EAdventure(std::string msg);
};

class EAdventureObjectAliasNotFound : public EAdventure
{
public:
    EAdventureObjectAliasNotFound(std::string alias);
};

class EAdventureObjectNameNotFound : public EAdventure
{
public:
    EAdventureObjectNameNotFound(std::string name);
};

