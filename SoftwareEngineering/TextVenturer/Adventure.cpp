#include "stdafx.h"       

#include "CmdLine.h"
#include "AdventureObject.h"
#include "Room.h"
#include "Item.h"
#include "Location.h"
#include "AdventureStructure.h"
#include "RoomConnection.h"
#include "Player.h"
#include "TextDisplay.h"
#include "FileFinder.h"
#include "CustomAdventureAction.h"

#include "Adventure.h"

void Adventure::initDefaultCommands()
{
    helpCommand.addAlias("help");

    showInventoryCommand.addAlias("inventory");
    showInventoryCommand.addAlias("show inventory");
    showInventoryCommand.addAlias("show my inventory");
    showInventoryCommand.addAlias("look in to inventory");
    showInventoryCommand.addAlias("list inventory");
    showInventoryCommand.addAlias("list my inventory");
    showInventoryCommand.addAlias("check inventory");
    showInventoryCommand.addAlias("check out inventory");
    showInventoryCommand.addAlias("what do I have on me");
    showInventoryCommand.addAlias("what items do I have on me");
    showInventoryCommand.addAlias("what am I carrying");
    showInventoryCommand.addAlias("what items am I carrying");

    lookAroundCommand.addAlias("look around");
    lookAroundCommand.addAlias("take a look around");
    lookAroundCommand.addAlias("ls");
    lookAroundCommand.addAlias("dir");
    lookAroundCommand.addAlias("explore");

    inspectCommand.addAlias("inspect <thing>");
    inspectCommand.addAlias("check out <thing>");
    inspectCommand.addAlias("investigate <thing>");
    inspectCommand.addAlias("search <thing>");
    inspectCommand.addAlias("examine <thing>");
    inspectCommand.addAlias("analyze <thing>");
    inspectCommand.addAlias("analyse <thing>");
    inspectCommand.addAlias("look into <thing>");
    inspectCommand.addAlias("look at <thing>");

    takeFromCommand.addAlias("take <item> <prep> <location>");
    takeFromCommand.addAlias("pick up <item> <prep> <location>");
    takeFromCommand.addAlias("pickup <item> <prep> <location>");
    takeFromCommand.addAlias("get <item> <prep> <location>");

    takeCommand.addAlias("take <item>");
    takeCommand.addAlias("pick up <item>");
    takeCommand.addAlias("pickup <item>");
    takeCommand.addAlias("get <item>");

    placeCommand.addAlias("place <item> <prep> <location>");
    placeCommand.addAlias("put <item> <prep> <location>");
    placeCommand.addAlias("lay <item> <prep> <location>");
    placeCommand.addAlias("deposit <item> <prep> <location>");
    placeCommand.addAlias("plop down <item> <prep> <location>");

    useRoomConnectionCommand.addAlias("go through <door>");
    useRoomConnectionCommand.addAlias("pass through <door>");
    useRoomConnectionCommand.addAlias("walk through <door>");
    useRoomConnectionCommand.addAlias("run through <door>");
    useRoomConnectionCommand.addAlias("get through <door>");

    gotoCommand.addAlias("go to <place>");
    gotoCommand.addAlias("walk to <place>");
    gotoCommand.addAlias("run to <place>");
    gotoCommand.addAlias("get to <place>");

    enterRoomCommand.addAlias("enter <room>");
    enterRoomCommand.addAlias("go in <room>");
    enterRoomCommand.addAlias("walk in <room>");
    enterRoomCommand.addAlias("step in <room>");
    enterRoomCommand.addAlias("go inside <room>");
    enterRoomCommand.addAlias("walk inside <room>");
    enterRoomCommand.addAlias("step inside <room>");
    enterRoomCommand.addAlias("go into <room>");
    enterRoomCommand.addAlias("walk into <room>");
    enterRoomCommand.addAlias("step into <room>");
    enterRoomCommand.addAlias("take a step into <room>");

    combineItemsCommand.addAlias("combine <item1> with <item2>");
    combineItemsCommand.addAlias("combine <item1> and <item2>");

	exitCommand.addAlias("exit");
	exitCommand.addAlias("kill");
	exitCommand.addAlias("stop");
	exitCommand.addAlias("terminate");
	exitCommand.addAlias("end");
	exitCommand.addAlias("let me out");
	exitCommand.addAlias("leave me alone");
	exitCommand.addAlias("/kill");
	exitCommand.addAlias("suicide");
	exitCommand.addAlias("ausmarsch");
	exitCommand.addAlias("resign");
	exitCommand.addAlias("quit");

    commandSystem.add(helpCommand, helpAction);
    commandSystem.add(lookAroundCommand, lookAroundAction);
    commandSystem.add(showInventoryCommand, showInventoryAction);
    commandSystem.add(inspectCommand, inspectAction);
    commandSystem.add(takeFromCommand, takeFromAction);
    commandSystem.add(takeCommand, takeAction);
    commandSystem.add(placeCommand, placeAction);
    commandSystem.add(gotoCommand, gotoAction);
    commandSystem.add(enterRoomCommand, enterRoomAction);
    commandSystem.add(combineItemsCommand, combineItemsAction);
    commandSystem.add(useRoomConnectionCommand, useRoomConnectionAction);
	commandSystem.add(exitCommand, exitAction);
}

void Adventure::loadFromStructure(const AdventureStructure::RootNode & root)
{
    namespace AS = AdventureStructure;
    
    auto getItems = [&](AS::ListNode & base, const std::string & name)
    {
        ref_vector<Item> result;
        for (std::string itemName : base.getStringList(name, true))
        {
            try
            {
                result.push_back(dynamic_cast<Item&>(findObjectByName(itemName)));
            }
            catch (std::bad_cast)
            {
                errorLog.push_back(ErrorLogEntry(base, "\"" + itemName + "\" in \"" + name + "\" is not an item"));
            }
            catch (EAdventureObjectNameNotFound)
            {
                errorLog.push_back(ErrorLogEntry(base, "\"" + itemName + "\" in \"" + name + "\" does not exist"));
            }
        }
        return result;
    };

    auto getLocations = [&](AS::ListNode & base, const std::string & name)
    {
        ref_vector<Location> result;
        for (std::string locationName : base.getStringList(name, true))
        {
            try
            {
                result.push_back(dynamic_cast<Location&>(findObjectByName(locationName)));
            }
            catch (std::bad_cast)
            {
                errorLog.push_back(ErrorLogEntry(base, "\"" + locationName + "\" in \"" + name + "\" is not a location"));
            }
            catch (EAdventureObjectNameNotFound)
            {
                errorLog.push_back(ErrorLogEntry(base, "\"" + locationName + "\" in \"" + name + "\" does not exist"));
            }
        }
        return result;
    };

    auto loadAdventureObject = [&](AS::ListNode & base, AdventureObject & object)
    {
        try
        {
            object.getAliases() = base.getAliases();
        }
        catch (const AS::EAdventureStructure & e)
        {
            errorLog.push_back(e);
        }

        try
        {
            object.setDescription(base.getString("Description"));
        }
        catch (const AS::EAdventureStructure & e)
        {
            errorLog.push_back(e);
        }
    };

    // getLocationItems
    auto getLocationItems = [&](AS::ListNode & base, Location & location)
    {
        if (AS::ListNode * itemsNode = base.tryGetListNode("Items"))
        {
            for (AS::BaseNode & node : *itemsNode)
            {
                try
                {
                    AS::ListNode & itemNode = dynamic_cast<AS::ListNode&>(node);

                    stringlist prepList, prepTake, itemNames;

                    Location::MultiInventory & inv = location.addInventory(itemNode.getName());

                    for (std::string alias : itemNode.getStringList("List", false, true))
                    {
                        inv.addPrepositionAlias(alias);
                        commandSystem.addPreposition(alias);
                    }

                    for (std::string alias : itemNode.getStringList("Take"))
                    {
                        inv.addPrepositionAlias(alias, true);
                        commandSystem.addPreposition(alias);
                    }

                    if (itemNode.hasChild("Whitelist"))
                    {
                        if (itemNode.hasChild("Blacklist"))
                            errorLog.push_back(ErrorLogEntry(itemNode, "Whitelist and Blacklist at the same time"));
                        else
                            inv.setFilterMode(Location::MultiInventory::ifWhitelist);
                    }
                    ref_vector<Item> items = getItems(itemNode, "Whitelist");
                    for (Item & item : items)
                        inv.addToFilter(item);

                    for (Item & item : getItems(itemNode, "Blacklist"))
                        inv.addToFilter(item);

                    for (Item & item : getItems(itemNode, "Items"))
                        inv.addItemForce(item);
                }
                catch (std::bad_cast)
                {
                    errorLog.push_back(AS::EWrongType(node, AS::ListNode::generateTypeName()));
                }
            }
        }
    };

    // getCustomCommands
    auto getCustomCommands = [&](AS::ListNode & base, CommandArray & result)
    {
        if (AS::ListNode * list = base.tryGetListNode("CustomCommands"))
        {
            for (AS::BaseNode & node : *list)
            {
                try
                {
                    AS::ListNode & itemNode = dynamic_cast<AS::ListNode&>(node);

                    stringlist aliases = itemNode.getStringList("Aliases");
                    std::string code = itemNode.getString("Action", AS::StringNode::stCode);
                    Command& cmd = *new Command();
                    for (std::string alias : aliases)
                        cmd.addAlias(alias);
                    try
                    {
                        CustomAdventureAction& action = *new CustomAdventureAction(*this, code, itemNode.getName());
                        result.add(cmd, action);
                    }
                    catch (const CustomScript::ECompile & e)
                    {
                        errorLog.push_back(ErrorLogEntry(node, e));
                    }
                }
                catch (std::bad_cast)
                {
                    errorLog.push_back(AS::EWrongType(node, AS::ListNode::generateTypeName()));
                }
            }
        }
    };

    // getEventCommand
    auto getEventCommand = [&](const AS::ListNode & base, std::string name) -> CustomAdventureAction*
    {
        if (AS::ListNode * typed = base.tryGetListNode(name))
        {
            bool overrideDefault = typed->getBoolean("Override");
            std::string code = typed->getString("Action", AS::StringNode::stCode);
            return new CustomAdventureAction(*this, code, name, overrideDefault);
        }
        return NULL;
    };

    // --- Start Reading ---
    // Title
    try
    {
        title = root.getString("Title");
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e);
    }

    try
    {
        description = root.getString("Description");
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e);
    }

    // Items
    if (AS::ListNode * itemsNode = root.tryGetListNode("Items"))
    {
        for (AS::BaseNode & node : *itemsNode)
        {
            try
            {
                AS::ListNode & itemNode = dynamic_cast<AS::ListNode&>(node);

                if (objectExists(itemNode.getName()))
                {
                    errorLog.push_back(ErrorLogEntry(itemNode, itemNode.getName() + " exists already"));
                    continue;
                }

                Item & item = *new Item();
                objects[itemNode.getName()] = &item;

                loadAdventureObject(itemNode, item);

                getCustomCommands(itemNode, item.getCarryCommands());

                item.setOnInspect(getEventCommand(itemNode, "OnInspect"));
                item.setOnTake(getEventCommand(itemNode, "OnTake"));
                item.setOnPlace(getEventCommand(itemNode, "OnPlace"));

            }
            catch (std::bad_cast)
            {
                errorLog.push_back(AS::EWrongType(node, AS::ListNode::generateTypeName()));
            }
        }
    }

    // Locations
    if (AS::ListNode * locationsNode = root.tryGetListNode("Locations"))
    {
        for (AS::BaseNode & node : *locationsNode)
        {
            try
            {
                AS::ListNode & locationNode = dynamic_cast<AS::ListNode&>(node);

                if (objectExists(locationNode.getName()))
                {
                    errorLog.push_back(ErrorLogEntry(locationNode, locationNode.getName() + " exists already"));
                    continue;
                }

                Location & location = *new Location();
                objects[locationNode.getName()] = &location;

                loadAdventureObject(locationNode, location);

                getLocationItems(locationNode, location);

                getCustomCommands(locationNode, location.getLocatedCommands());

                location.setOnInspect(getEventCommand(locationNode, "OnInspect"));
                location.setOnGoto(getEventCommand(locationNode, "OnGoto"));
                location.setOnLeave(getEventCommand(locationNode, "OnLeave"));
            }
            catch (std::bad_cast)
            {
                errorLog.push_back(AS::EWrongType(node, AS::ListNode::generateTypeName()));
            }
        }
    }

    // Rooms
    if (AS::ListNode * roomsNode = root.tryGetListNode("Rooms"))
    {
        for (AS::BaseNode & node : *roomsNode)
        {
            try
            {
                AS::ListNode & roomNode = dynamic_cast<AS::ListNode&>(node);

                if (objectExists(roomNode.getName()))
                {
                    errorLog.push_back(ErrorLogEntry(roomNode, roomNode.getName() + " exists already"));
                    continue;
                }

                Room & room = *new Room();
                objects[roomNode.getName()] = &room;

                loadAdventureObject(roomNode, room);

                for (Location & location : getLocations(roomNode, "Locations"))
                    room.addLocation(location);

                getCustomCommands(roomNode, room.getLocatedCommands());

                room.setOnInspect(getEventCommand(roomNode, "OnInspect"));
                room.setOnEnter(getEventCommand(roomNode, "OnEnter"));
                room.setOnLeave(getEventCommand(roomNode, "OnLeave"));
            }
            catch (std::bad_cast)
            {
                errorLog.push_back(AS::EWrongType(node, AS::ListNode::generateTypeName()));
            }
        }
    }

    // RoomConnections
    if (AS::ListNode * connectionsNode = root.tryGetListNode("RoomConnections"))
    {
        for (AS::BaseNode & node : *connectionsNode)
        {
            try
            {
                AS::ListNode & connectionNode = dynamic_cast<AS::ListNode&>(node);

                Room * rooms[2];
                bool success = true;
                for (int i = 0; i < 2; i++)
                {
                    try
                    {
                        std::string roomName = connectionNode.getString("Room" + std::to_string(i + 1), AS::StringNode::stIdent);
                        try
                        {
                            Room & room = dynamic_cast<Room&>(findObjectByName(roomName));
                            rooms[i] = &room;
                        }
                        catch (std::bad_cast)
                        {
                            errorLog.push_back(ErrorLogEntry(node, "\"" + roomName + "\" is not a room"));
                            success = false;
                        }
                    }
                    catch (const AS::EAdventureStructure & e)
                    {
                        errorLog.push_back(e);
                        success = false;
                    }
                }

                if (success)
                {
                    if (objectExists(connectionNode.getName()))
                    {
                        errorLog.push_back(ErrorLogEntry(connectionNode, connectionNode.getName() + " exists already"));
                        continue;
                    }

                    RoomConnection& connection = *new RoomConnection(*rooms[0], *rooms[1], !connectionNode.getBoolean("Locked"));
                    objects[connectionNode.getName()] = &connection;

                    loadAdventureObject(connectionNode, connection);

                    getLocationItems(connectionNode, connection);

                    getCustomCommands(connectionNode, connection.getLocatedCommands());

                    connection.setOnInspect(getEventCommand(connectionNode, "OnInspect"));
                    connection.setOnUse(getEventCommand(connectionNode, "OnUse"));
                }
            }
            catch (std::bad_cast)
            {
                errorLog.push_back(AS::EWrongType(node, AS::ListNode::generateTypeName()));
            }
        }
    }

    // ItemCombinations
    if (AS::ListNode * combosNode = root.tryGetListNode("ItemCombinations"))
    {
        for (AS::BaseNode & node : *combosNode)
        {
            AS::ListNode & itemComboNode = dynamic_cast<AS::ListNode&>(node);

            Item * comboItems[3];
            bool success = true;
            for (int i = 0; i < 3; i++)
            {
                std::string nodeName = i == 2 ? "Output" : "Input" + std::to_string(i + 1);
                try
                {
                    std::string itemName = itemComboNode.getString(nodeName, AS::StringNode::stIdent);
                    try
                    {
                        comboItems[i] = &dynamic_cast<Item&>(findObjectByName(itemName));
                    }
                    catch (std::bad_cast)
                    {
                        errorLog.push_back(ErrorLogEntry(node, "\"" + itemName + "\" is not an item"));
                        success = false;
                    }
                }
                catch (const AS::EAdventureStructure & e)
                {
                    errorLog.push_back(e);
                    success = false;
                }
            }

            if (success)
            {
                CustomAdventureAction * action = getEventCommand(itemComboNode, "OnCombine");
                itemCombiner.addCombination(*comboItems[0], *comboItems[1], *comboItems[2], action);
            }
        }
    }

    onInit = getEventCommand(root, "OnInit");

    // StartRoom
    startRoom = NULL;
    try
    {
        std::string startRoomName = root.getString("StartRoom", AS::StringNode::stIdent);
        try
        {
            startRoom = &dynamic_cast<Room&>(findObjectByName(startRoomName));
        }
        catch (std::bad_cast)
        {
            errorLog.push_back(ErrorLogEntry(root, "Specified StartRoom \"" + startRoomName + "\" is not a room"));
        }
        catch (EAdventureObjectNameNotFound)
        {
            errorLog.push_back(ErrorLogEntry(root, "Specified StartRoom \"" + startRoomName + "\" does not exist"));
        }
    }
    catch (const AS::ENodeNotFound & e)
    {
        errorLog.push_back(e);
    }

    for (AS::BaseNode & node : root.getUnusedNodes())
        errorLog.push_back(AS::EAdventureStructure(node, "Unknown identifier \"" + node.getName() + "\""));
}

Adventure::Adventure()
    : defaultAction(*this)
    , helpAction(*this)
    , showInventoryAction(*this)
    , lookAroundAction(*this)
    , inspectAction(*this)
    , takeFromAction(*this)
    , takeAction(*this)
    , placeAction(*this)
    , useRoomConnectionAction(*this)
    , gotoAction(*this)
    , enterRoomAction(*this)
    , combineItemsAction(*this)
    , exitAction(*this)
    , commandSystem(defaultAction)
{
    initDefaultCommands();

    initialized = false;
    running = false;

    onInit = NULL;
    player = NULL;  
}

Adventure::Adventure(std::wstring filename)
    : Adventure()
{                   
    std::wstring ext = extractFileExtension(filename);
    if (ext == L"txvs")
        loadScript(filename);
    if (ext == L"txvc")
        loadState(filename);
}

Adventure::~Adventure()
{
    for (auto & entry : objects)
        delete entry.second;

    delete player;       
    delete onInit;
}

void Adventure::loadScript(std::wstring filename)
{
    AdventureStructure::RootNode root;

    try
    {
        root.loadFromFile(filename);
    }
    catch (const AdventureStructure::EAdventureStructure & e)
    {
        errorLog.push_back(e);
    }             

    loadFromStructure(root);
    
    initialized = errorLog.empty();

    if (initialized)
        player = new Player("Player 1", *startRoom, commandSystem);    
}

void Adventure::loadState(std::wstring filename)
{
    FileStream stream(filename, std::ios::in);

    stream.read(title);
    stream.read(description);

    AdventureLoadHelp help(*this);
    
    UINT objectCount;
    stream.read(objectCount);
    for (UINT i = 0; i < objectCount; i++)
    {
        std::string name;
        stream.read(name);
        switch (AdventureObject::Type(stream.readUInt()))
        {
        case AdventureObject::otItem:
            help.objects.push_back(*new Item(stream, help));
            break;
        case AdventureObject::otLocation:
            help.objects.push_back(*new Location(stream, help));
        case AdventureObject::otRoom:
            help.objects.push_back(*new Room(stream, help));
            break;
            break;
        case AdventureObject::otRoomConnection:
            help.objects.push_back(*new RoomConnection(stream, help));
            break;
        }
        objects[name] = &help.objects.back().get();
    }

    commandSystem.load(stream, help);
    player = new Player(stream, commandSystem, help);
    itemCombiner.load(stream, help);

    stream.read(globalFlags);      
    stream.read(running);
    if (stream.readBool())
        onInit = new CustomAdventureAction(stream, *this);

    initialized = true;
}

bool Adventure::saveState(std::wstring filename) const
{
    if (!initialized)
        throw(EAdventureNotInitialized);

    FileStream stream(filename, std::ios::out);

    stream.write(title);
    stream.write(description);

    stream.write((UINT)objects.size());
    
    AdventureSaveHelp help;

    UINT id = 0; 
    for (auto & entry : objects)
    {
        stream.write(entry.first);
        stream.write(static_cast<UINT>(entry.second->getType()));
        help.objects[entry.second] = id++;
    }

    for (auto & entry : objects)
        if (dynamic_cast<Item*>(entry.second))
            entry.second->save(stream, help);    

    for (auto & entry : objects)
        if (dynamic_cast<Location*>(entry.second))
            entry.second->save(stream, help);

    for (auto & entry : objects)
        if (dynamic_cast<Room*>(entry.second))
            entry.second->save(stream, help);

    for (auto & entry : objects)
        if (dynamic_cast<RoomConnection*>(entry.second))
            entry.second->save(stream, help);

    commandSystem.save(stream, help);
    player->save(stream, help);
    itemCombiner.save(stream, help);

    stream.write(globalFlags);    
    stream.write(running);

    stream.write(onInit != NULL);
    if (onInit)
        onInit->save(stream);
    
    return true;
}

void Adventure::sendCommand(std::string command) 
{
    commandSystem.sendCommand(command);
}

Player & Adventure::getPlayer() 
{
    if (!player)
        throw(EAdventureNotInitialized);
    return *player;
}

ItemCombiner & Adventure::getItemCombiner()
{
    return itemCombiner;
}

CmdLine & Adventure::getCmdLine()
{
    if (!cmdLine)
        throw(EAdventureNotInitialized);
    return *cmdLine;
}

AdventureObject & Adventure::findObjectByAlias(std::string alias) const
{
    for (auto & entry : objects)
        if (entry.second->getAliases().has(alias))
            return *entry.second;
    throw(EAdventureObjectAliasNotFound, alias);
}

AdventureObject & Adventure::findObjectByName(std::string name) const
{
    for (auto & entry : objects)
        if (entry.first == name)
            return *entry.second;
    throw(EAdventureObjectNameNotFound, name);
}

bool Adventure::objectExists(std::string name) const
{
    for (auto & entry : objects)
        if (entry.first == name)
            return true;
    return false;
}

void Adventure::setFlag(std::string flag)
{
    globalFlags.insert(flag);
}

void Adventure::clearFlag(std::string flag)
{
    globalFlags.erase(flag);
}

void Adventure::toggleFlag(std::string flag)
{
    if (testFlag(flag))
        clearFlag(flag);
    else
        setFlag(flag);
}

bool Adventure::testFlag(std::string flag) const
{
    return globalFlags.find(flag) != globalFlags.end();
}

void Adventure::start(CmdLine & cmdLine)
{
    if (!initialized)
        throw(EAdventureNotInitialized);
    if (running)
        throw(EAdventureAlreadyRunning);
    
    this->cmdLine = &cmdLine;
    initDefaultCommands();
    if (!onInit || !onInit->overrides())
    {
        cmdLine.write("");
        cmdLine.write(title);
        cmdLine.write("");
        cmdLine.write(description);
        cmdLine.write("");
    }

    if (onInit)
        onInit->run();

    running = true;
}

bool Adventure::isInitialized() const
{
    return initialized;
}

bool Adventure::isRunning() const
{
    return running;
}

void Adventure::update()
{
    if (running)
        commandSystem.update();
}

std::string Adventure::getTitle() const
{
    return title;
}

std::string Adventure::getDescription() const
{
    return description;
}

const std::vector<Adventure::ErrorLogEntry> & Adventure::getErrorLog()
{
    return errorLog;
}

EAdventureObjectAliasNotFound::EAdventureObjectAliasNotFound(const std::string & alias)
    : EAdventure("AdventureObject alias \"" + alias + "\" not found")
{
}

EAdventure::EAdventure(std::string msg)
    : Exception(msg)
{
}

EAdventureObjectNameNotFound::EAdventureObjectNameNotFound(const std::string & name)
    : EAdventure("AdventureObject name \"" + name + "\" not found")
{
}

Adventure::ErrorLogEntry::ErrorLogEntry(const AdventureStructure::BaseNode & location, std::string msg)
    : location(location)
    , msg(msg)
    , type(etGenericError)
{
}

Adventure::ErrorLogEntry::ErrorLogEntry(const AdventureStructure::EAdventureStructure & exception)
    : location(exception.getNode())
    , msg(exception.what())
    , type(etStructureError)
{
}

Adventure::ErrorLogEntry::ErrorLogEntry(const AdventureStructure::BaseNode & location, const CustomScript::ECompile & exception)
    : location(location)
    , msg(exception.what())
    , type(etScriptError) 
{
}

EAdventureNotInitialized::EAdventureNotInitialized()
    : EAdventure("Adventure not initialized")
{
}

EAdventureAlreadyRunning::EAdventureAlreadyRunning()
    : EAdventure("Adventure already running")
{
}

AdventureLoadHelp::AdventureLoadHelp(Adventure & adventure)
    : adventure(adventure)
{
}
