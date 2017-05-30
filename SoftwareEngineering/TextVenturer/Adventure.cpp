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

    AS::ListNode* itemList = NULL;
    AS::ListNode* locationList = NULL;
    AS::ListNode* roomList = NULL;
    AS::ListNode* connectionList = NULL;
    AS::ListNode* itemComboList = NULL;
    
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
    auto checkEmpty = [&](const AS::ListNode & listnode)
    {
        for (const AS::BaseNode & node : listnode)
            errorLog.push_back(AS::EAdventureStructure(listnode, "Unknown identifier " + node.getName()));
    };
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

    // getLocationItems
    auto getLocationItems = [&](AS::ListNode* base, Location* location)
    {
        AS::ListNode* itemsNode = NULL;
        if (getList(base, "Items", itemsNode, false))
        {
            for (AS::BaseNode* baseItem : *itemsNode)
            {
                if (AS::ListNode* itemNode = *baseItem)
                {
                    stringlist prepList, prepTake, itemNames;
                    
                    Location::MultiInventory* inv = location->addInventory(itemNode->getName());
                    if (!inv)
                    {
                        error("Multiple inventories with same preposition. This error should not be able to occur?");
                        continue;
                    }
                                         
                    if (getStringList(itemNode, "List", false, prepList))
                    {
                        for (std::string alias : prepList)
                        {
                            inv->addPrepositionAlias(alias);
                            commandSystem.addPreposition(alias);
                        }
                    }                        
                   
                    if (getStringList(itemNode, "Take", false, prepTake, false))
                    {
                        for (std::string alias : prepTake)
                        {
                            inv->addPrepositionAlias(alias, true);
                            commandSystem.addPreposition(alias);
                        }
                    }

                    if (getStringList(itemNode, "Items", true, itemNames, false))
                    {
                        for (std::string itemName : itemNames)
                        {
                            try
                            {
                                try
                                {
                                    Item & item = dynamic_cast<Item&>(findObjectByName(itemName));
                                    inv->addItem(item);
                                }
                                catch (std::bad_cast)
                                {
                                    errorWrongType(itemList, itemName, AS::StringNode::getTypeName(AS::StringNode::stString));
                                }
                            }
                            catch (EAdventureObjectNameNotFound)
                            {
                                errorMissing(itemList, itemName, AS::StringNode::getTypeName(AS::StringNode::stString));
                            }
                        }
                    }

                    if ((AS::EmptyListNode*)*itemNode->get("Whitelist"))
                    {
                        inv->enableFilter(Location::MultiInventory::ifWhitelist);
                    }                        
                    else if (getStringList(itemNode, "Whitelist", true, itemNames, false))
                    {
                        inv->enableFilter(Location::MultiInventory::ifWhitelist);
                    }

                    if (itemNode->get("Blacklist") && inv->isFiltered())
                    {
                        error(itemNode->getFullPath() + " can't have a blacklist, since it already has a whitelist!");
                    }
                    else if (getStringList(itemNode, "Blacklist", true, itemNames, false))
                    {
                        inv->enableFilter(Location::MultiInventory::ifBlacklist);
                    }
                     
                    if (inv->isFiltered())
                    {
                        for (std::string itemName : itemNames)
                        {
                            if (Item* item = dynamic_cast<Item*>(findObjectByName(itemName)))
                            {
                                inv->addToFilter(item);
                            }
                            else
                            {
                                errorMissing(itemList, itemName, AS::StringNode::getTypeName(AS::StringNode::stString));
                            }
                        }
                    }
                }
                else if ((AS::EmptyListNode*)*baseItem)
                {
                    errorEmptyList(baseItem, AS::ListNode::getContentName());
                }
                else
                {
                    errorWrongType(baseItem, AS::ListNode::getTypeName());
                }
            }
            delete itemsNode;                                 
        }
    };

    // getCustomCommands
    auto getCustomCommands = [&](const AS::ListNode & base, CommandArray & result)
    {
        try
        {
            for (const AS::BaseNode & node : base.getListNode("CustomCommands"))
            {
                const AS::ListNode & itemNode = dynamic_cast<const AS::ListNode&>(node));
                
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
                catch (ETodo)
                {
                
                }

                checkEmpty(itemNode);
            }
        }
        catch (AS::ENodeNotFound)
        {
        }
    };

    // getEventCommand
    auto getEventCommand = [&](AS::ListNode & base, std::string name)
    {
        AS::ListNode & typed = base.getListNode(name);
        bool overrideDefault = typed.getBoolean("Override");
        std::string code = typed.getString("Action", AS::StringNode::stCode);
        return *new CustomAdventureAction(*this, code, name, overrideDefault);
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
        for (const AS::BaseNode & base : root.getListNode("Items"))
        {
            const AS::ListNode & itemNode = dynamic_cast<const AS::ListNode&>(base);

            if (objectExists(itemNode.getName()))
            {
                errorLog.push_back(ErrorLogEntry(itemNode, itemNode.getName() + " exists already"));
                continue;
            }
            Item& item = *new Item();
            objects[itemNode.getName()] = item;

            item.getAliases() = itemNode.getAliases();           
            item.setDescription(itemNode.getString("Description"));

            // CarryCommands
            getCustomCommands(itemNode, item.getCarryCommands());

            // Events
            CustomAdventureAction* action;
            // OnInspect
                item->setOnInspect(getEventCommand(itemNode, "OnInspect"));
            // OnTake
            if (getEventCommand(itemNode, "OnTake", action))
                item->setOnTake(action);
            // OnPlace
            if (getEventCommand(itemNode, "OnPlace", action))
                item->setOnPlace(action);

            checkEmpty(itemNode);
        }
    }
    catch (AS::ENodeNotFound)
    {
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e);
    }

    // Locations
    try
    {
        if (getList(&root, "Locations", locationList, false))
        {
            for (AS::BaseNode* base : *locationList)
            {
                if (AS::ListNode* locationNode = *base)
                {
                    if (findObjectByName(locationNode->getName()))
                    {
                        errorSameName(locationNode->getName());
                        continue;
                    }
                    Location* location = new Location();
                    objects[locationNode->getName()] = location;

                    // Aliases
                    getAliases(locationNode, location->getAliases());

                    // Description   
                    std::string description;
                    getString(locationNode, "Description", description);
                    location->setDescription(description);

                    // Items
                    getLocationItems(locationNode, location);

                    // CustomCommands
                    getCustomCommands(locationNode, location->getLocatedCommands());

                    // Events
                    CustomAdventureAction* action;
                    // OnInspect
                    if (getEventCommand(locationNode, "OnInspect", action))
                        location->setOnInspect(action);
                    // OnGoto
                    if (getEventCommand(locationNode, "OnGoto", action))
                        location->setOnGoto(action);
                    // OnLeave
                    if (getEventCommand(locationNode, "OnLeave", action))
                        location->setOnLeave(action);

                    checkEmpty(locationNode);
                }
                else if (AS::EmptyListNode* empty = *base)
                {
                    errorEmptyList(empty, AS::ListNode::getContentName());
                }
                else
                {
                    errorWrongType(base, AS::ListNode::getTypeName());
                }
            }
        }
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e.what());
    }

    // Rooms
    try
    {
        if (getList(&root, "Rooms", roomList))
        {
            for (AS::BaseNode* base : *roomList)
            {
                if (AS::ListNode* roomNode = *base)
                {
                    if (findObjectByName(roomNode->getName()))
                    {
                        errorSameName(roomNode->getName());
                        continue;
                    }
                    Room* room = new Room();
                    objects[roomNode->getName()] = room;

                    // Aliases
                    getAliases(roomNode, room->getAliases());

                    // Description
                    std::string description;
                    getString(roomNode, "Description", description);
                    room->setDescription(description);

                    // Locations
                    stringlist roomLocations;
                    if (getStringList(roomNode, "Locations", true, roomLocations, false))
                        for (std::string locationName : roomLocations)
                        {
                            if (Location* location = dynamic_cast<Location*>(findObjectByName(locationName)))
                            {
                                room->addLocation(location);
                            }
                            else
                            {
                                errorMissing(locationList, locationName, AS::ListNode::getTypeName());
                            }
                        }

                    // CustomCommands
                    getCustomCommands(roomNode, room->getLocatedCommands());

                    // Events
                    CustomAdventureAction* action;
                    // OnInspect
                    if (getEventCommand(roomNode, "OnInspect", action))
                        room->setOnInspect(action);
                    // OnEnter
                    if (getEventCommand(roomNode, "OnEnter", action))
                        room->setOnEnter(action);
                    // OnLeave
                    if (getEventCommand(roomNode, "OnLeave", action))
                        room->setOnLeave(action);

                    checkEmpty(roomNode);
                }
                else if (AS::EmptyListNode* empty = *base)
                {
                    errorEmptyList(empty, AS::ListNode::getContentName());
                }
                else
                {
                    errorWrongType(base, AS::ListNode::getTypeName());
                }
            }
        }
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e.what());
    }

    // RoomConnections
    try
    {
        for (const AS::BaseNode * node : root.getListNode("RoomConnections"))
        if (getList(&root, "RoomConnections", connectionList, false))
        {
            for (AS::BaseNode* base : *connectionList)
            {
                if (AS::ListNode* connectionNode = *base)
                {
                    Room* connectionRooms[2];
                    bool success = true;
                    for (int i = 0; i < 2; i++)
                    {
                        std::string roomName;
                        if (getString(connectionNode, "Room" + std::to_string(i + 1), roomName, AS::StringNode::stIdent))
                        {
                            if (Room* room = dynamic_cast<Room*>(findObjectByName(roomName)))
                            {
                                connectionRooms[i] = room;
                            }
                            else
                            {
                                errorMissing(roomList, roomName, AS::ListNode::getTypeName());
                                success = false;
                            }
                        }
                        else
                        {
                            errorMissing(connectionNode, "Room" + std::to_string(i + 1), AS::StringNode::getTypeName(AS::StringNode::stIdent));
                        }
                    }

                    std::string description;
                    getString(connectionNode, "Description", description);

                    AliasList aliases;
                    getAliases(connectionNode, aliases);

                    if (success)
                    {
                        if (findObjectByName(connectionNode->getName()))
                        {
                            errorSameName(connectionNode->getName());
                            continue;
                        }
                        RoomConnection* connection = new RoomConnection();
                        objects[connectionNode->getName()] = connection;

                        connection->getAliases() = aliases;
                        connection->setDescription(description);
                        getLocationItems(connectionNode, connection);

                        bool locked;
                        getBool(connectionNode, "Locked", locked);
                        connection->setConnection(connectionRooms[0], connectionRooms[1], !locked);

                        for (int i = 0; i < 2; i++)
                            connectionRooms[i]->addLocation(connection);

                        // CustomCommands
                        getCustomCommands(connectionNode, connection->getLocatedCommands());

                        // Events
                        CustomAdventureAction* action;
                        // OnInspect
                        if (getEventCommand(connectionNode, "OnInspect", action))
                            connection->setOnInspect(action);
                        // OnUse
                        if (getEventCommand(connectionNode, "OnUse", action))
                            connection->setOnUse(action);
                    }

                    checkEmpty(connectionNode);
                }
                else if (AS::EmptyListNode* empty = *base)
                {
                    errorEmptyList(empty, AS::ListNode::getContentName());
                }
                else
                {
                    errorWrongType(base, AS::ListNode::getTypeName());
                }
            }
        }
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e.what());
    }

    // ItemCombinations
    try
    {
        if (getList(&root, "ItemCombinations", itemComboList, false))
        {
            for (AS::BaseNode* base : *itemComboList)
            {
                if (AS::ListNode* itemComboNode = *base)
                {
                    Item* comboItems[3];
                    bool success = true;
                    for (int i = 0; i < 3; i++)
                    {
                        std::string itemName;
                        std::string nodeName = i == 2 ? "Output" : "Input" + std::to_string(i + 1);
                        if (getString(itemComboNode, nodeName, itemName, AS::StringNode::stIdent))
                        {
                            if (Item* item = dynamic_cast<Item*>(findObjectByName(itemName)))
                            {
                                comboItems[i] = item;
                            }
                            else
                            {
                                errorMissing(roomList, itemName, AS::ListNode::getTypeName());
                                success = false;
                            }
                        }
                        else
                        {
                            errorMissing(itemComboNode, nodeName, AS::StringNode::getTypeName(AS::StringNode::stIdent));
                        }
                    }

                    if (success)
                    {
                        CustomAdventureAction* action = NULL;
                        getEventCommand(itemComboNode, "OnCombine", action);
                        itemCombiner->addCombination(comboItems[0], comboItems[1], comboItems[2], action);
                    }

                    checkEmpty(itemComboNode);
                }
                else if (AS::EmptyListNode* empty = *base)
                {
                    errorEmptyList(empty, AS::ListNode::getContentName());
                }
                else
                {
                    errorWrongType(base, AS::ListNode::getTypeName());
                }
            }
        }
    }
    catch (const AS::EAdventureStructure & e)
    {
        errorLog.push_back(e.what());
    }

    // StartRoom
    std::string startRoomName;
    Room* startRoom = NULL;
    getString(root, "StartRoom", startRoomName, AS::StringNode::stIdent);
    if (Room* room = dynamic_cast<Room*>(findObjectByName(startRoomName)))
    {
        startRoom = room;
    }
    else if (roomList)
    {
        errorMissing(roomList, startRoomName, AS::ListNode::getTypeName());
    }

    // OnInit
    getEventCommand(&root, "OnInit", onInit);

    delete itemList;
    delete roomList;
    delete locationList;
    delete connectionList;
    delete itemComboList;

    checkEmpty(root);

    if (!errorLog.empty())
    {
        ErrorDialog("Adventure loading-error", errorLog);
        initialized = false;
        return false;
    }
    else
    {
        // loading success!
        player = new Player("Player 1", startRoom, commandSystem);
        initialized = true;
        return true;
    }    
}

void Adventure::loadState(std::wstring filename)
{
    FileStream stream(filename, std::ios::in);
    if (!stream.is_open())
        throw(ETodo);

    stream.read(title);
    stream.read(description);

    ref_vector<AdventureObject> objectList;
    ref_vector<CommandArray> commandArrayList;
    
    UINT objectCount;
    stream.read(objectCount);
    for (UINT i = 0; i < objectCount; i++)
    {
        std::string name;
        stream.read(name);
        AdventureObject* object;
        switch (AdventureObject::Type(stream.readUInt()))
        {
        case AdventureObject::otRoom:
            object = new Room();
            break;
        case AdventureObject::otLocation:
            object = new Location();
            break;
        case AdventureObject::otRoomConnection:
            object = new RoomConnection();
            break;
        case AdventureObject::otItem:
            object = new Item();
            break;
        }
        objectList.push_back(object);
        objects[name] = object;
    }

    for (auto entry : objectList)
    {
        entry.get().load(stream, *this, objectList, commandArrayList);
    }

    commandSystem->load(stream, commandArrayList);
    player = new Player(stream, commandSystem, objectList);
    itemCombiner->load(stream, this, objectList);

    stream.read(globalFlags);      
    stream.read(running);
    if (stream.readBool())
        onInit = new CustomAdventureAction(stream, this);

    initialized = true;
    return true;
}

bool Adventure::saveState(std::wstring filename) const
{
    if (!initialized)
        return false;

    FileStream stream(filename, std::ios::out);
    if (!stream.is_open())
        return false;

    stream.write(title);
    stream.write(description);

    stream.write((UINT)objects.size());
    
    idlist<AdventureObject&> objectIDs;
    idlist<CommandArray*> commandArrayIDs;

    UINT id = 0; 
    for (auto entry : objects)
    {
        stream.write(entry.first);
        stream.write(static_cast<UINT>(entry.second.getType()));
        objectIDs[entry.second] = id++;
    }

    for (auto entry : objects)
    {
        entry.second.save(stream, objectIDs, commandArrayIDs);
    }

    commandSystem->save(stream, commandArrayIDs);
    player->save(stream, objectIDs);
    itemCombiner->save(stream, objectIDs);

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
    if (player)
        return *player;
    throw(E);
}

ItemCombiner & Adventure::getItemCombiner()
{
    return itemCombiner;
}

CmdLine & Adventure::getCmdLine()
{
    return cmdLine;
}

AdventureObject & Adventure::findObjectByAlias(std::string alias) const
{
    for (auto entry : objects)
        if (entry.second.getAliases().has(alias))
            return entry.second;
    throw(EAdventureObjectAliasNotFound, alias);
}

AdventureObject & Adventure::findObjectByName(std::string name) const
{
    for (auto entry : objects)
        if (entry.first == name)
            return entry.second;
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

Adventure::ErrorLogEntry::ErrorLogEntry(const AdventureStructure::BaseNode & node, std::string msg)
    : node(node)
    , msg(msg)
{
}

Adventure::ErrorLogEntry::ErrorLogEntry(const AdventureStructure::EAdventureStructure & exception)
    : node(exception.getNode())
    , msg(exception.what())
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
