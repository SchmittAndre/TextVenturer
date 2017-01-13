#include "stdafx.h"
#include "Controler.h"
#include "Player.h"
#include "Room.h"
#include "Location.h"
#include "Item.h"
#include "CommandSystem.h"

#include "Adventure.h"

Adventure::Adventure(Controler * controler)
{
    player = NULL;

    Room* forest = new Room("Forest", "A dark,");

    player = new Player("Player 1", );
}

Adventure::~Adventure()
{
    delete player;
}

void Adventure::loadFromFile(string filename)
{
    // once we have this, I will be sooo happy! T_T
}

void Adventure::sendCommand(string command)
{
}

Player * Adventure::getPlayer()
{
    return player;
}
