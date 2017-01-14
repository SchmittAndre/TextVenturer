#include "stdafx.h"
#include "Controler.h"
#include "Player.h"
#include "Room.h"
#include "RoomConnection.h"
#include "Location.h"
#include "Container.h"
#include "Item.h"
#include "Inventory.h"
#include "Command.h"
#include "CommandSystem.h"         
#include "DefaultAdventureAction.h"
#include "ItemCombiner.h"

#include "Adventure.h"

Adventure::Adventure(Controler * controler, string filename)
{
    this->controler = controler;

    defaultAction = new DefaultAdventureAction(this);

    helpAction = new HelpAction(this);
    showInventoryAction = new ShowInventoryAction(this);
    lookAroundAction = new LookAroundAction(this);
    inspectAction = new InspectAction(this);
    pickupAction = new PickupAction(this);
    changeRoomAction = new UseRoomConnectionAction(this);
    gotoLocationAction = new GotoAction(this);
    combineItemsAction = new CombineItemsAction(this);
    useItemAction = new UseItemAction(this);     

    lockLocationAction = new LockLocationAction(this);
    unlockLocationAction = new UnlockLocationAction(this);

    helpCommand = new Command("help");
    
    showInventoryCommand = new Command("inventory");
    showInventoryCommand->addAlias("show inventory");
    showInventoryCommand->addAlias("list inventory");
    showInventoryCommand->addAlias("look into inventory");

    lookAroundCommand = new Command("look around");
    lookAroundCommand->addAlias("take a look around");
    lookAroundCommand->addAlias("ls");
    
    inspectCommand = new Command("inspect <location>");
    inspectCommand->addAlias("check out <location>");
    
    pickupCommand = new Command("pick up <item>");
    pickupCommand->addAlias("pickup <item>");
    pickupCommand->addAlias("take <item>");
    
    changeRoomCommand = new Command("go through <door>");
    changeRoomCommand->addAlias("use <door>");

    gotoLocationCommand = new Command("go to <location>");

    combineItemsCommand = new Command("combine <item1> with <item2>");
    combineItemsCommand->addAlias("combine <item1> and <item2>");

    useItemCommand = new Command("use <item> with <location>");
    useItemCommand->addAlias("use <item> and <location>");

    commandSystem = new CommandSystem(controler, defaultAction);
    commandSystem->add(helpCommand, helpAction);
    commandSystem->add(lookAroundCommand, lookAroundAction);
    commandSystem->add(showInventoryCommand, showInventoryAction);
    commandSystem->add(inspectCommand, inspectAction);
    commandSystem->add(pickupCommand, pickupAction);
    commandSystem->add(gotoLocationCommand, gotoLocationAction);
    commandSystem->add(combineItemsCommand, combineItemsAction);
    commandSystem->add(useItemCommand, useItemAction);
    commandSystem->add(changeRoomCommand, changeRoomAction);

    itemCombiner = new ItemCombiner();

    player = NULL;

    if (filename == "DEBUG")
        DEBUG_loadTest();
    else
        loadFromFile(filename);   
}

Adventure::~Adventure()
{
    delete defaultAction;
    
    delete helpAction;
    delete showInventoryAction;
    delete lookAroundAction;
    delete inspectAction;
    delete pickupAction;
    delete changeRoomAction;
    delete gotoLocationAction;
    delete combineItemsAction;
    delete useItemAction;

    delete lockLocationAction;
    delete unlockLocationAction;
    
    delete helpCommand;
    delete showInventoryCommand;
    delete lookAroundCommand;
    delete inspectCommand;
    delete pickupCommand;
    delete changeRoomCommand;
    delete gotoLocationCommand;
    delete combineItemsCommand;
    delete useItemCommand;

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

void Adventure::loadFromFile(string filename)
{
    // once we have this, I will be sooo happy! T_T
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
    Item* string = new Item("string", "A long piece of string.");
    Item* stick = new Item("stick", "It's a stick.");
    Item* bow = new Item("bow", "Not the best, but good enough.");

    // add some locations
    Container* well = new Container("well", "The handle fits and you could lift the bucket up.", false, "There is a bucket deep down, but the handle is missing.");
    Container* shelf = new Container("shelf", "Now you can reach the top of the shelf without problem.", false, "The shelf is tall, you can't reach the topmost layer.");
    Container* chest = new Container("chest", "The chest is open, revealing it's amazing loot!", false, "A chest, guess what? IT'S LOCKED!");
    Container* bush = new Container("bush", "The bush is in the middle of the garden.");

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
    well->addItemAction(handle, unlockLocationAction);
    shelf->addItemAction(box, unlockLocationAction);
    chest->addItemAction(key, unlockLocationAction);
    trapdoor->addItemAction(hammer, unlockLocationAction);

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
    getControler()->textscrolling("You are standing in " + player->currentRoom()->getName() + ".");
    
    initialized = true;
}

void Adventure::sendCommand(string command) const
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
