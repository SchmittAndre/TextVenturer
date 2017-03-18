#include "stdafx.h"       

#include "Controler.h"
#include "DefaultAdventureAction.h"
#include "Command.h"
#include "CommandSystem.h"
#include "ItemCombiner.h"
#include "AdventureObject.h"
#include "Room.h"
#include "Item.h"
#include "Location.h"
#include "AdventureStructure.h"
#include "RoomConnection.h"
#include "Player.h"
#include "TextDisplay.h"
#include "CustomAdventureAction.h"

#include "Adventure.h"

Adventure::Adventure(Controler * controler)
{
    this->controler = controler;
    initialized = false;

    defaultAction = new DefaultAdventureAction(this);   
    helpAction = new HelpAction(this);
    showInventoryAction = new ShowInventoryAction(this);
    lookAroundAction = new LookAroundAction(this);
    inspectAction = new InspectAction(this);
    takeFromAction = new TakeFromAction(this);
    takeAction = new TakeAction(this);
    placeAction = new PlaceAction(this);
    useRoomConnectionAction = new UseRoomConnectionAction(this);
    gotoAction = new GotoAction(this);
    enterRoomAction = new EnterRoomAction(this);
    combineItemsAction = new CombineItemsAction(this);

    helpCommand = new Command("help");

    showInventoryCommand = new Command("inventory");
    showInventoryCommand->addAlias("show inventory");
    showInventoryCommand->addAlias("show my inventory");
    showInventoryCommand->addAlias("look in to inventory");
    showInventoryCommand->addAlias("list inventory");
    showInventoryCommand->addAlias("list my inventory");
    showInventoryCommand->addAlias("check inventory");
    showInventoryCommand->addAlias("check out inventory");
    showInventoryCommand->addAlias("what do I have on me");
    showInventoryCommand->addAlias("what items do I have on me");
    showInventoryCommand->addAlias("what am I carrying");
    showInventoryCommand->addAlias("what items am I carrying");

    lookAroundCommand = new Command("look around");
    lookAroundCommand->addAlias("take a look around");
    lookAroundCommand->addAlias("ls");
	lookAroundCommand->addAlias("dir");
    lookAroundCommand->addAlias("explore");

    inspectCommand = new Command("inspect <thing>");
    inspectCommand->addAlias("check out <thing>");
    inspectCommand->addAlias("investigate <thing>");
    inspectCommand->addAlias("search <thing>");
    inspectCommand->addAlias("examine <thing>");
    inspectCommand->addAlias("analyze <thing>");
    inspectCommand->addAlias("analyse <thing>");
    inspectCommand->addAlias("look into <thing>");

    takeFromCommand = new Command("take <item> <prep> <location>");
    takeFromCommand->addAlias("pick up <item> <prep> <location>");
    takeFromCommand->addAlias("pickup <item> <prep> <location>");
    takeFromCommand->addAlias("get <item> <prep> <location>");

    takeCommand = new Command("take <item>");
    takeCommand->addAlias("pick up <item>");
    takeCommand->addAlias("pickup <item>");
    takeCommand->addAlias("get <item>");

    placeCommand = new Command("place <item> <prep> <location>");
    placeCommand->addAlias("put <item> <prep> <location>");
    placeCommand->addAlias("lay <item> <prep> <location>");
    placeCommand->addAlias("deposit <item> <prep> <location>");
    placeCommand->addAlias("plop down <item> <prep> <location>");

    useRoomConnectionCommand = new Command("go through <door>");
    useRoomConnectionCommand->addAlias("pass through <door>");
    useRoomConnectionCommand->addAlias("walk through <door>");
    useRoomConnectionCommand->addAlias("run through <door>");
    useRoomConnectionCommand->addAlias("get through <door>");

    gotoCommand = new Command("go to <place>");
    gotoCommand->addAlias("walk to <place>");
    gotoCommand->addAlias("run to <place>");
    gotoCommand->addAlias("get to <place>");

    enterRoomCommand = new Command("enter <room>");
    enterRoomCommand->addAlias("go in <room>");
    enterRoomCommand->addAlias("walk in <room>");
    enterRoomCommand->addAlias("step in <room>");
    enterRoomCommand->addAlias("go inside <room>");
    enterRoomCommand->addAlias("walk inside <room>");
    enterRoomCommand->addAlias("step inside <room>");
    enterRoomCommand->addAlias("go into <room>");
    enterRoomCommand->addAlias("walk into <room>");
    enterRoomCommand->addAlias("step into <room>");
    enterRoomCommand->addAlias("take a step into <room>");

    combineItemsCommand = new Command("combine <item1> with <item2>");
    combineItemsCommand->addAlias("combine <item1> and <item2>");

    commandSystem = new CommandSystem(controler, defaultAction);
    commandSystem->add(helpCommand, helpAction);
    commandSystem->add(lookAroundCommand, lookAroundAction);
    commandSystem->add(showInventoryCommand, showInventoryAction);
    commandSystem->add(inspectCommand, inspectAction);
    commandSystem->add(takeFromCommand, takeFromAction);
    commandSystem->add(takeCommand, takeAction);
    commandSystem->add(placeCommand, placeAction);
    commandSystem->add(gotoCommand, gotoAction);
    commandSystem->add(enterRoomCommand, enterRoomAction);
    commandSystem->add(combineItemsCommand, combineItemsAction);
    commandSystem->add(useRoomConnectionCommand, useRoomConnectionAction);

    itemCombiner = new ItemCombiner();

    player = NULL;   
}

Adventure::~Adventure()
{
    delete player;

    delete commandSystem;
    delete defaultAction;

    delete itemCombiner;

    for (auto entry : objects)
        delete entry.second;
}

bool Adventure::loadFromFile(std::string filename)
{
    namespace AS = AdventureStructure;
    AS::RootNode root;
    if (!root.loadFromFile(filename))
        return false;

    std::string errorLog;
    size_t errorCount = 0;

    AS::ListNode* itemList = NULL;
    AS::ListNode* locationList = NULL;
    AS::ListNode* roomList = NULL;
    AS::ListNode* connectionList = NULL;
    AS::ListNode* itemComboList = NULL;
    
    // --- Error Functions ---
    // error
    auto error = [&](std::string msg)
    {
        if (!errorLog.empty())
            errorLog += "\n";
        errorLog += std::to_string(++errorCount) + ") " + msg;
    };

    // errorMissing
    auto errorMissing = [&](AS::ListNode* node, std::string name, std::string type)
    {
        error("\"" + node->getFullPath() + "/" + name + "\" missing! (Type: " + type + ")");
    };

    // errorWrongType
    auto errorWrongType = [&](AS::BaseNode* node, std::string reqType, std::string gotType = "")
    {
        if (gotType != "")
            error("\"" + node->getFullPath() + "\" is \"" + gotType + "\" and should be \"" + reqType + "\"!");
        else
            error("\"" + node->getFullPath() + "\" must be \"" + reqType + "\"!");
    };

    // errorEmptyList
    auto errorEmptyList = [&](AS::BaseNode* node, std::string reqContent)
    {
        error("\"" + node->getFullPath() + "\" is empty and should contain " + reqContent + "!");
    };

    // errorSameName
    auto errorSameName = [&](std::string name)
    {
        error("Object with identifier \"" + name + "\" exists multiple times!");
    };

    auto errorCompile = [&](AS::BaseNode* node)
    {
        error("Could not compile \"" + node->getFullPath() + "\"!");
    };

    // --- Help Functions ---

    // checkEmpty
    auto checkEmpty = [&](AS::ListNode* listnode)
    {
        if (listnode->empty())
            return true;

        std::string err = "Unknown identifier";

        if (listnode->getCount() == 1)
        {
            err += " " + (*listnode->begin())->getFullPath();
        }
        else
        {
            err += "s in " + listnode->getFullPath() + "/..";
            for (AS::BaseNode* node : *listnode)
            {
                err += "\n     - " + node->getName();
            }
        }
        error(err);
        return false;
    };

    // getString
    auto getString = [&](AS::ListNode* base, std::string name, std::string & result, AS::StringNode::Type type = AS::StringNode::stString, bool required = true)
    {
        if (AS::BaseNode* node = base->get(name))
        {
            if (AS::StringNode* typed = *node)
            {
                if (typed->getType() == type)
                {
                    result = *typed;
                    delete typed;
                    return true;
                }
                else if (required)
                {
                    errorWrongType(node, AS::StringNode::getTypeName(type), AS::StringNode::getTypeName(typed->getType()));
                    delete typed;
                }
            }
            else if (required)
            {
                errorWrongType(node, AS::StringNode::getTypeName(type));
            }
        }
        else if (required)
        {
            errorMissing(base, name, AS::StringNode::getTypeName(type));
        }
        return false;
    };

    // getBool
    auto getBool = [&](AS::ListNode* base, std::string name, bool& result, bool required = false, bool defaultValue = false)
    {
        if (AS::BaseNode* node = base->get(name))
        {
            if (AS::StringNode* typed = *node)
            {
                if (typed->getType() == AS::StringNode::stIdent)
                {
                    if (typed->getValue() == "true")
                    {
                        result = true;
                        delete typed;
                        return true;
                    }
                    else if (typed->getValue() == "false")
                    {
                        result = false;
                        delete typed;
                        return true;
                    }
                }
                errorWrongType(typed, "[true/false]", typed->getValue());                
            }
            else
            {
                errorWrongType(node, "[true/false]", node->getTypeName());
            }
            delete node;
        }
        else if (!required)
        {
            result = defaultValue;
            return true;
        }
        else
        {
            errorMissing(base, name, AS::StringNode::getTypeName(AS::StringNode::stIdent));
        }
        return false;
    };

    // getList
    auto getList = [&](AS::ListNode* base, std::string name, AS::ListNode* &result, bool required = true)
    {
        if (AS::BaseNode* node = base->get(name))
        {
            if (result = *node)
            {
                return true;
            }
            else if ((AS::EmptyListNode*)*node)
            {
                errorEmptyList(node, AS::ListNode::getContentName());
                delete node;
            }
            else if (required)
            {
                errorWrongType(node, AS::ListNode::getTypeName());
            }
        }
        else if (required)
        {
            errorMissing(base, name, AS::ListNode::getTypeName());
        }
        return false;
    };

    // getStringList
    auto getStringList = [&](AS::ListNode* base, std::string name, bool identList, strings &result, bool required = true)
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
    auto getAliases = [&](AS::ListNode* base, AliasList& result)
    {
        strings aliases;
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
                    strings prepList, prepTake, itemNames;
                    
                    Location::PInventory* inv = location->addInventory(itemNode->getName());
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
                            commandSystem->addPreposition(alias);
                        }
                    }                        
                   
                    if (getStringList(itemNode, "Take", false, prepTake, false))
                    {
                        for (std::string alias : prepTake)
                        {
                            inv->addPrepositionAlias(alias, true);
                            commandSystem->addPreposition(alias);
                        }
                    }

                    if (getStringList(itemNode, "Items", true, itemNames, false))
                    {
                        for (std::string itemName : itemNames)
                        {
                            if (Item* item = dynamic_cast<Item*>(findObjectByName(itemName)))
                            {
                                inv->addItem(item);
                            }
                            else
                            {
                                errorMissing(itemList, itemName, AS::StringNode::getTypeName(AS::StringNode::stString));
                            }
                        }
                    }

                    if ((AS::EmptyListNode*)*itemNode->get("Whitelist"))
                    {
                        inv->enableFilter(Location::PInventory::ifWhitelist);
                    }                        
                    else if (getStringList(itemNode, "Whitelist", true, itemNames, false))
                    {
                        inv->enableFilter(Location::PInventory::ifWhitelist);
                    }

                    if (itemNode->get("Blacklist") && inv->isFiltered())
                    {
                        error(itemNode->getFullPath() + " can't have a blacklist, since it already has a whitelist!");
                    }
                    else if (getStringList(itemNode, "Blacklist", true, itemNames, false))
                    {
                        inv->enableFilter(Location::PInventory::ifBlacklist);
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
    auto getCustomCommands = [&](AS::ListNode* base, CommandArray* result)
    {
        AS::ListNode* itemsNode = NULL;
        if (getList(base, "CustomCommands", itemsNode, false))
        {
            for (AS::BaseNode* itemBase : *itemsNode)
            {                                   
                if (AS::ListNode* itemNode = *itemBase)
                {
                    strings aliases;
                    std::string code;
                    if (!getStringList(itemNode, "Aliases", false, aliases))
                        continue;
                    if (!getString(itemNode, "Action", code, AS::StringNode::stCode))
                        continue;
                    Command* cmd = new Command(aliases[0]);
                    for (auto iter = aliases.begin() + 1; iter != aliases.end(); iter++)
                        cmd->addAlias(*iter);
                    CustomAdventureAction* action = new CustomAdventureAction(this, code, itemNode->getName());
                    if (!action->compileSucceeded())
                        errorCompile(itemNode);
                    if (!result->add(cmd, action))
                        delete action;
                    checkEmpty(itemNode);
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
            delete itemsNode;
        }
    };

    // getEventCommand
    auto getEventCommand = [&](AS::ListNode* base, std::string name, CustomAdventureAction* &result)
    {
        AS::BaseNode* node = base->get(name);
        if (AS::ListNode* typed = *node)
        {
            bool overrideDefault;
            std::string code;
            if (!getBool(typed, "Override", overrideDefault))
                return false;
            if (!getString(typed, "Action", code, AS::StringNode::stCode))
                return false;
            result = new CustomAdventureAction(this, code, name, overrideDefault);
            if (!result->compileSucceeded())
                errorCompile(typed);
            delete node;
            return true;
        }
        else if (AS::EmptyListNode* empty = *node)
        {
            errorEmptyList(empty, AS::ListNode::getContentName());
        }
        else if (node)
        {
            errorWrongType(node, AS::ListNode::getTypeName());
        }
        delete node;   
        return false;
    };

    // --- Start Reading ---
    // Title
    getString(&root, "Title", title);
    getString(&root, "Description", description);
    
    // Items
    if (getList(&root, "Items", itemList, false))
    {
        for (AS::BaseNode* base : *itemList)
        {
            if (AS::ListNode* itemNode = *base)
            {
                if (findObjectByName(itemNode->getName()))
                {
                    errorSameName(itemNode->getName());
                    continue;
                }
                Item* item = new Item();
                objects[itemNode->getName()] = item;

                // Aliases
                getAliases(itemNode, item->getAliases());

                // Description   
                std::string description;
                getString(itemNode, "Description", description);
                item->setDescription(description);

                // CarryCommands
                getCustomCommands(itemNode, item->getCarryCommands());

                // Events
                CustomAdventureAction* action;       
                // OnInspect
                if (getEventCommand(itemNode, "OnInspect", action))
                    item->setOnInspect(action);
                // OnTake
                if (getEventCommand(itemNode, "OnTake", action))
                    item->setOnTake(action);
                // OnPlace
                if (getEventCommand(itemNode, "OnPlace", action))
                    item->setOnPlace(action);

                checkEmpty(itemNode);
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

    // Locations
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

    // Rooms
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
                strings roomLocations;
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

    // RoomConnections
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
                    RoomConnection* connection = new RoomConnection(connectionRooms[0], connectionRooms[1]);
                    objects[connectionNode->getName()] = connection;

                    connection->getAliases() = aliases;
                    connection->setDescription(description);
                    getLocationItems(connectionNode, connection);

                    for (int i = 0; i < 2; i++)
                        connectionRooms[i]->addLocation(connection);

                    bool locked;
                    getBool(connectionNode, "Locked", locked);
                    if (locked)
                        connection->lock();

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

    // ItemCombinations
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

    delete itemList;
    delete roomList;
    delete locationList;
    delete connectionList;
    delete itemComboList;

    checkEmpty(root);

    if (!errorLog.empty())
    {
        ErrorDialog("Adventure loading-error", errorLog);
        return false;
    }
    else
    {
        // loading success!
        player = new Player("Player 1", startRoom, commandSystem);
        initialized = true;

        if (saveState("data\\compiled\\compiled.tvb"))
            controler->write("$lime()Successfully saved!");
        else
            controler->write("$red()Could not save!");

        return true;
    }    
}

bool Adventure::loadState(std::string filename)
{
    FileStream stream(filename, std::ios::in);
    if (!stream.is_open())
        return false;

    UINT length;

    stream.read(title);
    stream.read(description);

    stream.read(length);
    for (UINT i = 0; i < length; i++)
    {
        std::string key = stream.readString();
        // TODO: AdventureObject* object = AdventureObject::load(stream);
        //       objects[key] = object;
    }

    // TODO: commandSystem->load(stream);
    // TODO: player->load(stream);
    // TODO: itemCombiner->load(stream);

    stream.read(globalFlags);

    return false;
}

bool Adventure::saveState(std::string filename)
{
    FileStream stream(filename, std::ios::out);
    if (!stream.is_open())
        return false;

    stream.write(title);
    stream.write(description);

    stream.write((UINT)objects.size());
    idlist ids;
    UINT id = 0;
    for (auto entry : objects)
    {
        stream.write(entry.first);
        ids[entry.first] = id++;
    }

    for (auto entry : objects)
    {
        stream.write(entry.second->getType());
        entry.second->save(stream, ids);
    }

    commandSystem->save(stream);
    player->save(stream);
    itemCombiner->save(stream);

    stream.write(globalFlags);
    
    return true;
}

void Adventure::sendCommand(std::string command) const
{
    commandSystem->sendCommand(command);
}

Player * Adventure::getPlayer() const
{
    return player;
}

ItemCombiner * Adventure::getItemCombiner() const
{
    return itemCombiner;
}

Controler * Adventure::getControler() const
{
    return controler;
}

AdventureObject * Adventure::findObjectByAlias(std::string name) const
{
    for (auto entry : objects)
        if (entry.second->getAliases().has(name))
            return entry.second;
    return NULL;
}

AdventureObject * Adventure::findObjectByName(std::string name) const
{
    for (auto entry : objects)
        if (entry.first == name)
            return entry.second;
    return NULL;
}

void Adventure::setFlag(std::string flag)
{
    globalFlags.insert(flag);
}

void Adventure::clearFlag(std::string flag)
{
    globalFlags.erase(flag);
}

bool Adventure::testFlag(std::string flag)
{
    return globalFlags.find(flag) != globalFlags.end();
}

bool Adventure::isInitialized() const
{
    return initialized;
}

void Adventure::update() const
{
    commandSystem->update();
}
