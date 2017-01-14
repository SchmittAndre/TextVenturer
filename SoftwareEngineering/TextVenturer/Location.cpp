#include "stdafx.h"
#include "Item.h"
#include "Command.h"
#include "AdventureAction.h"

#include "Location.h"

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

string Location::getDescription() const
{
    return description;
}

void Location::changeDescription(string description)
{
    this->description = description;
}

bool Location::addItemAction(Item * item, AdventureAction * action)
{
    if (getItemAction(item))
        return false;
    useActions[item] = action;
    return true;
}

bool Location::delItemAction(Item * item)
{
    if (!getItemAction(item))
        return false;
    useActions[item] = NULL;
    return false;
}

AdventureAction * Location::getItemAction(Item * item) const
{
    unordered_map<Item*, AdventureAction*>::const_iterator found = useActions.find(item);
    return found == useActions.end() ? NULL : found->second;
}
