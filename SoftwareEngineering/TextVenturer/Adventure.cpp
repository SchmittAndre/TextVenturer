#include "stdafx.h"       

#include "Adventure.h"

#include "AdventureStructure.h"

Adventure::Adventure(Controler * controler)
{
    this->controler = controler;
    initialized = false;

    defaultAction = new DefaultAdventureAction(this);

    helpAction = new HelpAction(this);
    showInventoryAction = new ShowInventoryAction(this);
    lookAroundAction = new LookAroundAction(this);
    inspectAction = new InspectAction(this);
    pickupAction = new PickupAction(this);
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
    lookAroundCommand->addAlias("explore");
    
    inspectCommand = new Command("inspect <location>");
    inspectCommand->addAlias("check out <location>");
    inspectCommand->addAlias("investigate <location>");
    inspectCommand->addAlias("search <location>");
    inspectCommand->addAlias("examine <location>");
    inspectCommand->addAlias("analyze <location>");
    inspectCommand->addAlias("analyse <location>");
    inspectCommand->addAlias("look into <location>");
    
    pickupCommand = new Command("pick up <item>");
    pickupCommand->addAlias("pickup <item>");
    pickupCommand->addAlias("take <item>");
    pickupCommand->addAlias("get <item>");
    pickupCommand->addAlias("put <item> in inventory");
    pickupCommand->addAlias("put <item> in my inventory");
    pickupCommand->addAlias("put <item> into inventory");
    pickupCommand->addAlias("put <item> into my inventory");
    
    useRoomConnectionCommand = new Command("go through <door>");
    useRoomConnectionCommand->addAlias("pass through <door>");
    useRoomConnectionCommand->addAlias("walk through <door>");
    useRoomConnectionCommand->addAlias("run through <door>");
    useRoomConnectionCommand->addAlias("get through <door>");
    useRoomConnectionCommand->addAlias("use <door>");

    gotoCommand = new Command("go to <location>");
    gotoCommand->addAlias("walk to <location>");
    gotoCommand->addAlias("run to <location>");
    gotoCommand->addAlias("get to <location>");

    enterRoomCommand = new Command("enter <room>");
    enterRoomCommand->addAlias("step in <room>");
    enterRoomCommand->addAlias("step into <room>");
    enterRoomCommand->addAlias("take a step into <room>");

    combineItemsCommand = new Command("combine <item1> with <item2>");
    combineItemsCommand->addAlias("combine <item1> and <item2>");

    commandSystem = new CommandSystem(controler, defaultAction);
    commandSystem->add(helpCommand, helpAction);
    commandSystem->add(lookAroundCommand, lookAroundAction);
    commandSystem->add(showInventoryCommand, showInventoryAction);
    commandSystem->add(inspectCommand, inspectAction);
    commandSystem->add(pickupCommand, pickupAction);
    commandSystem->add(gotoCommand, gotoAction);
    commandSystem->add(enterRoomCommand, enterRoomAction);
    commandSystem->add(combineItemsCommand, combineItemsAction);
    commandSystem->add(useRoomConnectionCommand, useRoomConnectionAction);

    itemCombiner = new ItemCombiner();

    player = NULL;   
}

Adventure::~Adventure()
{
    delete defaultAction;
    
    delete helpAction;
    delete showInventoryAction;
    delete lookAroundAction;
    delete inspectAction;
    delete pickupAction;
    delete useRoomConnectionAction;
    delete gotoAction;
    delete enterRoomAction;
    delete combineItemsAction;
    
    delete helpCommand;
    delete showInventoryCommand;
    delete lookAroundCommand;
    delete inspectCommand;
    delete pickupCommand;
    delete useRoomConnectionCommand;
    delete gotoCommand;
    delete enterRoomCommand;
    delete combineItemsCommand;

    delete player;

    delete commandSystem;

    delete itemCombiner;

    for (std::pair<std::string, Room*> entry : rooms)
        delete entry.second;

    for (std::pair<std::string, Item*> entry : items)
        delete entry.second;

    for (std::pair<std::string, Location*> entry : locations)
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

    // --- Error Functions ---
    // error
    auto error = [&errorLog, &errorCount](std::string msg)
    {
        if (!errorLog.empty())
            errorLog += "\n";
        errorLog += std::to_string(++errorCount) + ") " + msg;
    };

    // errorMissing
    auto errorMissing = [&error](AS::ListNode* node, std::string name, std::string type)
    {
        error("\"" + node->getFullPath() + "/" + name + "\" missing! (Type: " + type + ")");
    };

    // errorWrongType
    auto errorWrongType = [&error](AS::BaseNode* node, std::string reqType, std::string gotType = "")
    {
        if (gotType != "")
            error("\"" + node->getFullPath() + "\" is \"" + gotType + "\" and should be \"" + reqType + "\"!");
        else
            error("\"" + node->getFullPath() + "\" must be \"" + reqType + "\"!");
    };

    // errorEmptyList
    auto errorEmptyList = [&error](AS::BaseNode* node, std::string reqContent)
    {
        error("\"" + node->getFullPath() + "\" is empty and should contain " + reqContent + "!");
    };

    // --- Help Functions ---
    // getString
    auto getString = [&errorMissing, &errorWrongType](AS::ListNode* base, std::string name, std::string & result, AS::StringNode::Type type = AS::StringNode::stString, bool required = true)
    {
        std::string e;
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

    // getList
    auto getList = [&errorMissing, &errorWrongType, &errorEmptyList](AS::ListNode* base, std::string name, AS::ListNode* &result, bool required = true)
    {
        if (AS::BaseNode* node = base->get(name))
        {
            if (result = (AS::ListNode*)*node)
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
    auto getStringList = [&errorMissing, &errorWrongType, &errorEmptyList](AS::ListNode* base, std::string name, bool identList, strings &result, bool required = true)
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
    auto getAliases = [&errorMissing, &getStringList](AS::ListNode* base, AliasList& result)
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

    // checkEmpty
    auto checkEmpty = [&error](AS::ListNode* listnode)
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

    // --- Start Reading ---
    // Title
    getString(&root, "Title", title);
    getString(&root, "Description", description);
    
    // Items
    AS::ListNode* itemList = NULL;
    if (getList(&root, "Items", itemList, false))
    {
        for (AS::BaseNode* base : *itemList)
        {
            if (AS::ListNode* itemNode = *base)
            {
                Item* item = new Item();
                items[itemNode->getName()] = item;

                // Aliases
                getAliases(itemNode, item->getAliases());

                // Description   
                std::string description;
                getString(itemNode, "Description", description);
                item->setDescription(description);

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
    AS::ListNode* locationList = NULL;
    if (getList(&root, "Locations", locationList, false))
    {
        for (AS::BaseNode* base : *locationList)
        {           
            if (AS::ListNode* locationNode = *base)
            {
                Location* location = new Location();
                locations[locationNode->getName()] = location;

                // Aliases
                getAliases(locationNode, location->getAliases());

                // Description   
                std::string description;
                getString(locationNode, "Description", description);
                location->setDescription(description); 

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
    AS::ListNode* roomList = NULL;
    if (getList(&root, "Rooms", roomList))
    {
        for (AS::BaseNode* base : *roomList)
        {
            if (AS::ListNode* roomNode = *base)
            {
                Room* room = new Room();
                rooms[roomNode->getName()] = room;

                // Aliases
                getAliases(roomNode, room->getAliases());

                // Description
                std::string description;
                getString(roomNode, "Description", description);
                room->setDescription(description);

                // Locations
                strings roomLocations;
                getStringList(roomNode, "Locations", true, roomLocations);
                for (std::string locationName : roomLocations)
                {
                    auto entry = locations.find(locationName);
                    if (entry != locations.end())
                    {
                        room->addLocation(entry->second);
                    }
                    else
                    {
                        errorMissing(locationList, locationName, AS::ListNode::getTypeName());
                    }
                }

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
    AS::ListNode* connectionList = NULL;
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
                    if (getString(connectionNode, "room" + std::to_string(i + 1), roomName, AS::StringNode::stIdent))
                    {
                        auto entry = rooms.find(roomName);
                        if (entry != rooms.end())
                        {
                            connectionRooms[i] = entry->second;
                        }
                        else
                        {
                            errorMissing(roomList, roomName, AS::ListNode::getTypeName());
                            success = false;
                        }
                    }
                    else
                    {
                        errorMissing(connectionNode, "room" + std::to_string(i + 1), AS::StringNode::getTypeName(AS::StringNode::stIdent));
                    }
                }

                std::string description;
                getString(connectionNode, "Description", description);

                AliasList aliases;
                getAliases(connectionNode, aliases);

                if (success)
                {
                    RoomConnection* connection = new RoomConnection(connectionRooms[0], connectionRooms[1]);
                    locations[connectionNode->getName()] = connection;
                    
                    connection->getAliases() = aliases;
                    connection->setDescription(description);  

                    for (int i = 0; i < 2; i++)
                        connectionRooms[i]->addLocation(connection);
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
    AS::ListNode* itemComboList = NULL;
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
                    std::string nodeName = i == 2 ? "output" : "input" + std::to_string(i + 1);
                    if (getString(itemComboNode, nodeName, itemName, AS::StringNode::stIdent))
                    {
                        auto entry = items.find(itemName);
                        if (entry != items.end())
                        {
                            comboItems[i] = entry->second;
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
                    itemCombiner->addCombination(comboItems[0], comboItems[1], comboItems[2]);

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
    auto entry = rooms.find(startRoomName);
    if (entry != rooms.end())
    {
        startRoom = entry->second;
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
        player = new Player("Player 1", startRoom);
        initialized = true;
        return true;
    }    
}

void Adventure::DEBUG_loadTest()
{
    // This is just for testing until the loadFromFile function is up and running
    /*
    // add some rooms
    Room* garden = new Room("garden", "The garden is filled with grass and flower patches.");
    garden->getAliases()->add("flower garden");
    Room* shed = new Room("shed", "This is a very run-down shed.");
    shed->getAliases()->add("old shed");
    Room* basement = new Room("basement", "Who builds a basement under a shed?");
    
    // add some items
    Item* handle = new Item("handle", "A handle, you could stick onto somewhere.");
    Item* key = new Item("key", "A rusty key.");
    Item* box = new Item("box", "A medium sized box, strong enough for you to stand on.");
    Item* hammer = new Item("hammer", "A heavy hammer.");
    Item* string = new Item("std::string", "A long piece of std::string.");
    Item* stick = new Item("stick", "It's a stick.");
    Item* bow = new Item("bow", "Not the best, but good enough.");

    // add some locations
    Location* well = new Location("well", "There is a bucket deep down, but the handle is missing.");
    Location* shelf = new Location("shelf", "The shelf is tall, you can't reach the topmost layer.");
    Location* chest = new Location("chest", "A chest, guess what? IT'S LOCKED!");
    Location* bush = new Location("bush", "The bush is in the middle of the garden.");

    // add the room connections
    RoomConnection* sheddoor = new RoomConnection("shed door", "This door is so broken, it can't stop me!", shed, garden);
    RoomConnection* trapdoor = new RoomConnection("trapdoor", "The trapdoor is completly ruined now... Oh well...", shed, basement, false, "This trapdoor looks surprisingly tough.");

    // add the item combinations
    itemCombiner->addCombination(string, stick, bow);

    // give the rooms some locations
    garden->addLocation(well);
    garden->addLocation(bush);
    shed->addLocation(shelf);
    basement->addLocation(chest);   

    garden->addLocation(sheddoor);
    shed->addLocation(trapdoor);

    // give the locations some items
    well->getInventory()->addItem(key); 
    bush->getInventory()->addItem(box);
    bush->getInventory()->addItem(hammer);
    shelf->getInventory()->addItem(handle);
    chest->getInventory()->addItem(string);
    chest->getInventory()->addItem(stick);

    // give the locked locations an unlock action     
    controler->write("TODO: unlock actions!"); // TODO: testing unlock actions

    // add everything to the corresponding list
    rooms.push_back(garden);
    rooms.push_back(shed);
    rooms.push_back(basement);

    items.push_back(handle);
    items.push_back(key);
    items.push_back(box);
    items.push_back(hammer);
    items.push_back(string);
    items.push_back(stick);
    items.push_back(bow);

    locations.push_back(well);
    locations.push_back(bush);
    locations.push_back(chest);
    locations.push_back(shelf);
    locations.push_back(sheddoor);
    locations.push_back(trapdoor);

    player = new Player("Player 1", garden);
    getControler()->write("You are standing in " + player->currentRoom()->getName(player) + ".");
    
    initialized = true;
    */
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

bool Adventure::isInitialized() const
{
    return initialized;
}

void Adventure::update() const
{
    commandSystem->update();
}
