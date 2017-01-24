#include "stdafx.h"
#include "Item.h"
#include "Command.h"
#include "CustomAdventureAction.h"
#include "Player.h"
#include "CommandSystem.h"

#include "Location.h"

Location::LocatedCommandAction::LocatedCommandAction(Command * command, CustomAdventureAction * action, bool anywhere)
:   commandAction(command, action)
{
    this->anywhere = anywhere;
}  

Location::Location(string name, string description)
{
    aliases = new AliasList(name);
    this->description = description;
}

Location::~Location()
{
    delete aliases;
}

AliasList* Location::getAliases() const
{
    return aliases;
}

string Location::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases->getName(definiteArticle, startOfSentence);
}

string Location::getName(Player * player, bool startOfSentence) const
{
    return getName(player->knows((Location*)this), startOfSentence);
}

string Location::getDescription() const
{
    return description;
}

void Location::changeDescription(string description)
{
    this->description = description;
}

void Location::addCommand(Command * command, CustomAdventureAction * action, bool anywhere)
{
    commands.push_back(LocatedCommandAction(command, action, anywhere));
}