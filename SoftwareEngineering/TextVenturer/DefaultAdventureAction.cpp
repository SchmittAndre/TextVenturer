#include "stdafx.h"
#include "Command.h"
#include "Controler.h"
#include "Adventure.h"

#include "DefaultAdventureAction.h"

void DefaultAdventureAction::run(Command::Result params)
{
    write("Wait... what?");
}

void HelpAction::run(Command::Result params)
{
    write("TODO!");
}

void ShowInventoryAction::run(Command::Result params)
{
    write("TODO!");
}

void InspectAction::run(Command::Result params)
{
    write("Your inventory contains:");
    for (Item* item : getAdventure()->)
    
    write("TODO!");
}

void PickupAction::run(Command::Result params)
{
    write("TODO!");
}

void ChangeRoomAction::run(Command::Result params)
{
    write("TODO!");
}

void GotoLocationAction::run(Command::Result params)
{
    write("TODO!");
}

void CombineItemsAction::run(Command::Result params)
{
    write("TODO!");
}

void UseItemAction::run(Command::Result params)
{
    write("TODO!");
}
