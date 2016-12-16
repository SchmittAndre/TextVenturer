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

AliasList* Item::getAliases()
{
    return aliases;
}

string Item::getDescription()
{
    return description;
}
