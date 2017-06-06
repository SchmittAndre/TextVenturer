#include "stdafx.h"

#include "Adventure.h"
#include "Player.h"
#include "CustomAdventureAction.h"
#include "AliasList.h"

#include "AdventureObject.h"

AdventureObject::AdventureObject(FileStream & stream, AdventureLoadHelp & help)
    : aliases(stream)
    , description(stream.readString())
    , flags(stream.readTags())
    , onInspect(CustomAdventureAction::loadConditional(stream, help.adventure))
{
}

AdventureObject::AdventureObject()
    : description("[no description]")
    , onInspect(NULL)
{
}

AdventureObject::~AdventureObject()
{
    delete onInspect;
}

AliasList & AdventureObject::getAliases()
{
    return aliases;
}

std::string AdventureObject::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases.getName(definiteArticle, startOfSentence);
}

std::string AdventureObject::getName(Player & player, bool startOfSentence) const
{
    return getName(player.knows(*this), startOfSentence);
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

CustomAdventureAction * AdventureObject::getOnInspect() const
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

void AdventureObject::toggleFlag(std::string flag)
{
    if (testFlag(flag))
        clearFlag(flag);
    else
        setFlag(flag);
}

bool AdventureObject::testFlag(std::string flag) const
{
    return flags.find(flag) != flags.end();
}

void AdventureObject::save(FileStream & stream, AdventureSaveHelp & help) const
{
    aliases.save(stream);
    stream.write(description);
    stream.write(flags);
    CustomAdventureAction::saveConditional(stream, onInspect);
}
