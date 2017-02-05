#include "stdafx.h"

#include "Player.h"
#include "Location.h"

Location::LocatedCommandAction::LocatedCommandAction(Command * command, CustomAdventureAction * action, bool anywhere)
:   commandAction(command, action)
{
    this->anywhere = anywhere;
}  

Location::Location()
{
    inventory = new Inventory();
}

Location::~Location()
{
    delete inventory;
}

AliasList& Location::getAliases()
{
    return aliases;
}


std::string Location::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases.getName(definiteArticle, startOfSentence);
}


std::string Location::getName(Player * player, bool startOfSentence) const
{
    return getName(player->knows((Location*)this), startOfSentence);
}


std::string Location::getDescription() const
{
    return description;
}

void Location::setDescription(std::string description)
{
    this->description = description;
}

void Location::addCommand(Command * command, CustomAdventureAction * action, bool anywhere)
{
    commands.push_back(LocatedCommandAction(command, action, anywhere));
}

Inventory * Location::getInventory()
{
    return inventory;
}
