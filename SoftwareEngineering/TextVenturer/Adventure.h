#pragma once

#include "DefaultAdventureAction.h"
#include "Command.h"
#include "CommandSystem.h"
#include "ItemCombiner.h"
#include "AdventureStructure.h"

class CmdLine;
class Player;
class AdventureObject;
class Adventure;

struct AdventureLoadHelp
{
    Adventure & adventure;
    ref_vector<AdventureObject> objects;
    ref_vector<CommandArray> commandArrays;

    AdventureLoadHelp(Adventure & adventure);
};

struct AdventureSaveHelp
{
    idlist<const AdventureObject*> objects;
    idlist<const CommandArray*> commandArrays;
};

class Adventure
{                   
private:

    struct ErrorLogEntry
    {
        const AdventureStructure::BaseNode & node;
        std::string msg;

        ErrorLogEntry(const AdventureStructure::BaseNode & node, std::string msg);
        ErrorLogEntry(const AdventureStructure::EAdventureStructure & exception);
    };

    // "constant"

    CmdLine* cmdLine;

    DefaultAdventureAction defaultAction;
    HelpAction helpAction;
    ShowInventoryAction showInventoryAction; 
    LookAroundAction lookAroundAction;
    InspectAction inspectAction; 
    TakeFromAction takeFromAction;
    TakeAction takeAction;
    PlaceAction placeAction;
    UseRoomConnectionAction useRoomConnectionAction; 
    GotoAction gotoAction; 
    EnterRoomAction enterRoomAction;
    CombineItemsAction combineItemsAction; 
	ExitAction exitAction;
    
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

    std::vector<ErrorLogEntry> errorLog;

    // state specific (must get saved)

    std::string title;
    std::string description;

    std::unordered_map<std::string, AdventureObject*> objects;
    
    CommandSystem commandSystem;
    Player * player;
    CustomAdventureAction * onInit;
    
    ItemCombiner itemCombiner;

    taglist globalFlags;

    bool running;     
                          
    void initDefaultCommands();

    void loadScript(std::wstring filename);
    void loadState(std::wstring filename);

    Adventure();

public:
    Adventure(std::wstring filename);
    virtual ~Adventure();

    bool saveState(std::wstring filename) const;

    void sendCommand(std::string command);

    Player & getPlayer();
    ItemCombiner & getItemCombiner();

    CmdLine & getCmdLine();

    AdventureObject & findObjectByAlias(std::string alias) const;
    AdventureObject & findObjectByName(std::string name) const;

    bool objectExists(std::string name) const;

    void setFlag(std::string flag);
    void clearFlag(std::string flag);
    void toggleFlag(std::string flag);
    bool testFlag(std::string flag) const;
    
    void start(CmdLine & cmdLine);

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

class EAdventureNotInitialized : public EAdventure
{
public:
    EAdventureNotInitialized();
};

class EAdventureAlreadyRunning : public EAdventure
{
public:
    EAdventureAlreadyRunning();
};

class EAdventureObjectAliasNotFound : public EAdventure
{
public:
    EAdventureObjectAliasNotFound(const std::string & alias);
};

class EAdventureObjectNameNotFound : public EAdventure
{
public:
    EAdventureObjectNameNotFound(const std::string & name);
};

