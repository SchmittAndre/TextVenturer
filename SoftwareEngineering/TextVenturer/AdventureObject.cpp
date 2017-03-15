#include "stdafx.h"

#include "Player.h"
#include "CustomAdventureAction.h"

#include "AdventureObject.h"

AdventureObject::AdventureObject()
{
    description = "No description!";
    onInspect = NULL;
}

AdventureObject::~AdventureObject()
{
    delete onInspect;
}

AliasList& AdventureObject::getAliases()
{
    return aliases;
}

std::string AdventureObject::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases.getName(definiteArticle, startOfSentence);
}

std::string AdventureObject::getName(Player * player, bool startOfSentence) const
{
    return getName(player->knows((AdventureObject*)this), startOfSentence);
}

std::string AdventureObject::getNameOnly(bool startOfSentence) const
{
    return aliases.getNameOnly(startOfSentence);
}

bool AdventureObject::isNamePlural() const
{
    return aliases.isNamePlural();
}

void AdventureObject::setDescription(std::string description)
{
    this->description = description;
}

std::string AdventureObject::getDescription() const
{
    return description;
}

CustomAdventureAction * AdventureObject::getOnInspect()
{
    return onInspect;
}

void AdventureObject::setOnInspect(CustomAdventureAction * onInspect)
{
    this->onInspect = onInspect;
}

void AdventureObject::setFlag(std::string flag)
{
    flags.insert(flag);
}

void AdventureObject::clearFlag(std::string flag)
{
    flags.erase(flag);
}

bool AdventureObject::testFlag(std::string flag)
{
    return flags.find(flag) != flags.end();
}