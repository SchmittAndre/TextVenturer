#include "stdafx.h"

#include "Item.h"

AliasList& Item::getAliases()
{
    return aliases;
}
          
std::string Item::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases.getName(definiteArticle, startOfSentence);
}

bool Item::isNamePlural() const
{
    return aliases.isNamePlural();
}

void Item::setDescription(std::string description)
{
    this->description = description;
}


std::string Item::getDescription() const
{
    return description;
}
