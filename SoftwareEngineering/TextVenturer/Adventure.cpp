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

    for (Room* room : rooms)
        delete room;

    for (Item* item : items)
        delete item;

    for (Location* location : locations)
        delete location;
}

bool Adventure::loadFromFile(std::string filename)
{
    namespace AS = AdventureStructure;
    AS::RootNode root("Adventure");
    if (!root.loadFromFile(filename))
        return false;

    // --- Help Functions ---
    // error
    auto error = [](std::string msg, std::string additional = "")
    {
        if (additional != "")
            additional = "\n> " + additional;
        ErrorDialog("Adventure loading", msg + additional);
    };

    // getString
    auto getString = [&](AS::BaseNode* node, std::string name, std::string & result, AS::StringNode::Type type = AS::StringNode::stString)
    {
        std::string e;
        if (AS::BaseNode* base = root.get(name))
        {
            if (AS::StringNode* typed = *base)
            {
                if (typed->getType() == type)
                {
                    result = *typed;
                    delete typed;
                    return true;
                }
                error(base->getName() + " is type " + typed->typeString() + 
                                        " and should be " + AS::StringNode::typeString(type), base->getFullPath());
            }
        }
        else
        {
            error(base->getFullPath() + "/" + name + " missing! (Type: " + AS::StringNode::typeString(type) + ")");
        }
        return false;
    };

    auto errorRest... // this should do the same thing, that is currently at the bottom of this function

    // --- Start Reading ---
    // Title
    if (!getString(&root, "Title", title))
        return false;

    // Description
    if (!getString(&root, "Description", description))
        return false;   

    // Rooms

    // ...

    if (!root.empty())
    {
        std::string err = "Unknown identifier";
        if (root.getCount() > 1)
            err += "s";
        err += ":";
        for (AS::BaseNode* node : root)
        {
            err += "\n- " + node->getName();
        }
        error(err);
    }

    return true;
}

void Adventure::DEBUG_loadTest()
{
    // This is just for testing until the loadFromFile function is up and running

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
    /*
    well->addCommand(, unlockLocationAction);
    shelf->addItemAction(box, unlockLocationAction);
    chest->addItemAction(key, unlockLocationAction);
    trapdoor->addItemAction(hammer, unlockLocationAction);
    */

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
