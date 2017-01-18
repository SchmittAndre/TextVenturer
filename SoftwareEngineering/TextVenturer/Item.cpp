#include "stdafx.h"

#include "Item.h"

Item::Item(string name, string description)
{
    aliases = new AliasList(name);
    this->description = description;
}

Item::~Item()
{
    delete aliases;
}

AliasList* Item::getAliases() const
{
    return aliases;
}

string Item::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases->getName(definiteArticle, startOfSentence);
}

bool Item::isNamePlural() const
{
    return aliases->aliases[0].isPlural();
}

string Item::getDescription() const
{
    return description;
}
