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
    namespace AS = AdventureStructure;
    AS::RootNode root;

    try
    {
        root.loadFromFile(filename);
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e);
    }

    // --- Error Functions ---
    /*
    // error
    auto error = [&](std::string msg)
    {
        if (!errorLog.empty())
            errorLog += "\n";
        errorLog += std::to_string(++errorCount) + ") " + msg;
    };

    // errorMissing
    auto errorMissing = [&](AS::ListNode & node, std::string name, std::string type)
    {
        error("\"" + node.getFullPath() + "/" + name + "\" missing! (Type: " + type + ")");
    };

    // errorWrongType
    auto errorWrongType = [&](AS::BaseNode & node, std::string reqType, std::string gotType = "")
    {
        if (gotType != "")
            error("\"" + node.getFullPath() + "\" is \"" + gotType + "\" and should be \"" + reqType + "\"!");
        else
            error("\"" + node.getFullPath() + "\" must be \"" + reqType + "\"!");
    };

    // errorEmptyList
    auto errorEmptyList = [&](AS::BaseNode & node, std::string reqContent)
    {
        error("\"" + node.getFullPath() + "\" is empty and should contain " + reqContent + "!");
    };

    // errorSameName
    auto errorSameName = [&](std::string name)
    {
        error("Object with identifier \"" + name + "\" exists multiple times!");
    };

    auto errorCompile = [&](AS::BaseNode & node)
    {
        error("Could not compile \"" + node.getFullPath() + "\"!");
    };
    */
    // --- Help Functions ---

    // checkEmpty
    /*
    auto checkEmpty = [&](AS::ListNode & listnode)
    {
        for (const AS::BaseNode & node : listnode)
            errorLog.push_back(AS::EAdventureStructure(listnode, "Unknown identifier " + node.getName()));
    };
    */
    /*
    // getString
    auto getString = [&](AS::ListNode& base, std::string name, AS::StringNode::Type type = AS::StringNode::stString, bool required = true)
    {
            AS::BaseNode& node = base.get(name);
            try
            {
                AS::StringNode& typed = dynamic_cast<AS::StringNode&>(node);
                if (typed.getType() == type)
                {
                    std::string result = typed;
                    typed.remove();
                    return result;
                }
                else if (required)
                {
                    throw(AS::EWrongType, base, typed, AS::StringNode::generateTypeName(type));
                    typed.remove();
                }
            }
            catch (std::bad_cast)
            {
                throw(AS::EWrongType, base, node, AS::StringNode::generateTypeName(type));
            }
        }
    };

    // getBool
    auto getBool = [&](AS::ListNode & base, std::string name, bool required = false, bool defaultValue = false)
    {
        try
        {
            AS::BaseNode& node = base.get(name);
            try
            {
                AS::StringNode& typed = dynamic_cast<AS::StringNode&>(node);
                if (typed.getType() == AS::StringNode::stIdent)
                {
                    if (typed.getValue() == "true")
                    {
                        typed.remove();
                        return true;
                    }
                    else if (typed.getValue() == "false")
                    {
                        typed.remove();
                        return false;
                    }
                }
                errorWrongType(typed, "[true/false]", typed.getValue());
            }
            catch (std::bad_cast)
            {
                errorWrongType(node, "[true/false]", node.generateTypeName());
            }
            node.remove();
        }
        catch (AS::ENodeNotFound)
        {
            if (!required)
            {
                return defaultValue;
            }
            else
            {
                errorMissing(base, name, AS::StringNode::getTypeName(AS::StringNode::stIdent));
            }
        }
    };

    // getList
    auto getList = [&](AS::ListNode & base, std::string name, bool required = true)
    {
        try
        {
            AS::BaseNode & node = base.get(name);
            try
            {
                return dynamic_cast<AS::ListNode&>(node);
            }
            catch (std::bad_cast)
            {
                try
                {
                    dynamic_cast<AS::EmptyListNode&>(node);
                }
                catch (std::bad_cast)
                {
                    errorEmptyList(node, AS::ListNode::getContentName());
                }
                node.remove();
            }
            if (required)
            {
                errorWrongType(node, AS::ListNode::getTypeName());
            }
        }
        catch (AS::ENodeNotFound)
        {
            if (required)
            {
                errorMissing(base, name, AS::ListNode::getTypeName());
            }
        }
    };

    // getStringList
    auto getStringList = [&](AS::ListNode* base, std::string name, bool identList, bool required = true)
    {
        if (AS::BaseNode* node = base->get(name))
        {
            if (AS::StringListNode* typed = *node)
            {
                if (typed->isIdentifierList() == identList)
                {
                    for (std::string alias : *typed)
                        result.push_back(alias);
                    delete node;
                    return true;
                }
                errorWrongType(node, AS::StringListNode::getTypeName(identList), AS::StringListNode::getTypeName(typed->isIdentifierList()));
            }
            else if (AS::EmptyListNode* empty = *node)
            {
                errorEmptyList(empty, AS::StringListNode::getContentName(identList));
            }
            else if (required)
            {
                errorWrongType(node, AS::StringListNode::getTypeName(identList));
            }
            delete node;
        }
        else if (required)
        {
            errorMissing(base, name, AS::StringListNode::getTypeName(identList));
        }
        return false;
    };

    // getAliases
    auto getAliases = [&](AS::ListNode* base)
    {
        stringlist aliases;
        bool found = false;
        if (getStringList(base, "Aliases", false, aliases, false))
        {
            for (std::string alias : aliases)
                result.add(alias);
            found = true;
        }
        if (getStringList(base, "PluralAliases", false, aliases, false))
        {
            for (std::string alias : aliases)
                result.add(alias);
            found = true;
        }
        if (!found)
            errorMissing(base, "(Plural)Aliases", AS::StringListNode::getTypeName(false));
        return found;
    };
    */

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
        try
        {
            AS::ListNode & itemsNode = base.getListNode("Items");
            for (AS::BaseNode & baseItem : itemsNode)
            {
                try
                {
                    AS::ListNode & itemNode = dynamic_cast<AS::ListNode&>(baseItem);

                    stringlist prepList, prepTake, itemNames;

                    Location::MultiInventory & inv = location.addInventory(itemNode.getName());

                    for (std::string alias : itemNode.getStringList("List"))
                    {
                        inv.addPrepositionAlias(alias);
                        commandSystem.addPreposition(alias);
                    }

                    try
                    {
                        for (std::string alias : itemNode.getStringList("Take"))
                        {
                            inv.addPrepositionAlias(alias, true);
                            commandSystem.addPreposition(alias);
                        }
                    }
                    catch (AS::ENodeNotFound) {}

                    try
                    {
                        ref_vector<Item> items = getItems(itemNode, "Whitelist");
                        if (itemNode.hasChild("Blacklist"))
                            errorLog.push_back(ErrorLogEntry(itemNode, "Whitelist and Blacklist at the same time"));
                        else
                        {
                            inv.setFilterMode(Location::MultiInventory::ifWhitelist);
                            for (Item & item : items)
                                inv.addToFilter(item);
                        }
                    }
                    catch (AS::ENodeNotFound)
                    {
                        try
                        {
                            for (Item & item : getItems(itemNode, "Blacklist"))
                            {
                                inv.addToFilter(item);
                            }
                        }
                        catch (AS::ENodeNotFound)
                        {
                            // not filtered
                        }
                    }
                }
                catch (std::bad_cast)
                {
                    throw(AS::EWrongType, base, baseItem, AS::ListNode::generateTypeName());
                }
            }
        }
        catch (AS::ENodeNotFound) {}
    };

    // getCustomCommands
    auto getCustomCommands = [&](AS::ListNode & base, CommandArray & result)
    {
        try
        {
            AS::ListNode & list = base.getListNode("CustomCommands");
            for (AS::BaseNode & node : list)
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

                    // checkEmpty(itemNode);
                }
                catch (std::bad_cast)
                {
                    errorLog.push_back(ErrorLogEntry(node, "Only Node-Lists are allowed inside of CustomCommands"));
                }
            }
        }
        catch (AS::ENodeNotFound)
        {
        }
    };

    // getEventCommand
    auto getEventCommand = [&](const AS::ListNode & base, std::string name) -> CustomAdventureAction*
    {
        try
        {
            AS::ListNode & typed = base.getListNode(name);
            bool overrideDefault = typed.getBoolean("Override");
            std::string code = typed.getString("Action", AS::StringNode::stCode);
            return new CustomAdventureAction(*this, code, name, overrideDefault);
        }
        catch (AS::ENodeNotFound)
        {
            return NULL;
        }
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
    try
    {
        AS::ListNode & itemsNode = root.getListNode("Items");
        for (AS::BaseNode & base : itemsNode)
        {
            AS::ListNode & itemNode = dynamic_cast<AS::ListNode&>(base);

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

            // checkEmpty(itemNode);
        }
    }
    catch (AS::ENodeNotFound) {}

    // Locations
    try
    {
        AS::ListNode & locationsNode = root.getListNode("Locations");
        for (AS::BaseNode & base : locationsNode)
        {
            AS::ListNode & locationNode = dynamic_cast<AS::ListNode&>(base);

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

            // checkEmpty(locationNode);
        }
    }
    catch (AS::ENodeNotFound) {}

    // Rooms
    try
    {
        AS::ListNode & roomsNode = root.getListNode("Rooms");
        for (AS::BaseNode & base : roomsNode)
        {
            AS::ListNode & roomNode = dynamic_cast<AS::ListNode&>(base);

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

            // checkEmpty(roomNode);
        }
    }
    catch (AS::ENodeNotFound) {}

    // RoomConnections
    try
    {
        AS::ListNode & connectionsNode = root.getListNode("RoomConnections");
        for (AS::BaseNode & node : connectionsNode)
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

            // checkEmpty(connectionNode);
        }
    }
    catch (AS::ENodeNotFound) {}

    // ItemCombinations
    try
    {
        AS::ListNode & combosNode = root.getListNode("ItemCombinations");
        for (AS::BaseNode & node : combosNode)
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
                        comboItems[i] = &dynamic_cast<Item&>(findObjectByName(nodeName));
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

            // checkEmpty(itemComboNode);
        }
    }
    catch (AS::ENodeNotFound) {}

    onInit = getEventCommand(root, "OnInit");

    // StartRoom
    Room * startRoom = NULL;
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

    // checkEmpty(root);

    for (AS::BaseNode & node : root.getUnusedNodes())
        errorLog.push_back(AS::EAdventureStructure(node, "Unknown identifier \"" + node.getName() + "\""));

    if (!errorLog.empty())
    {
        std::string debugString = "Errors preventing loading:";
        for (ErrorLogEntry & entry : errorLog)
            debugString += "\r\n- " + entry.msg + "\r\n  at " + entry.location.getFullPath();
        // TODO: Only show this in the seperate Displayer,
        // TODO: Show error count as description
        ErrorDialog("Adventure loading-error", debugString);
        initialized = false;
    }
    else
    {
        // loading success!
        player = new Player("Player 1", *startRoom, commandSystem);
        initialized = true;
    }    
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
    for (auto entry : objects)
        if (entry.first == name)
            return *entry.second;
    throw(EAdventureObjectNameNotFound, name);
}

bool Adventure::objectExists(std::string name) const
{
    for (auto entry : objects)
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
