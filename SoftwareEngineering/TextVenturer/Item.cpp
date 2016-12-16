#include "stdafx.h"

#include "Item.h"

Item::Item(string name)
{
    aliases.push_back(name);
}

bool Item::addAlias(string name)
{
    if (find(aliases.begin(), aliases.end(), name) != aliases.end())
        return false;
    aliases.push_back(name);
    return true;
}

bool Item::delAlias(string name)
{
    strings::iterator i = find(aliases.begin(), aliases.end(), name);
    if (i == aliases.end())
        return false;
    aliases.erase(i);
    return true;
}

bool Item::hasAlias(string name)
{
    return find(aliases.begin(), aliases.end(), name) != aliases.end();
}

string Item::getName()
{
    return aliases[0];
}

strings Item::getAliases()
{
    return aliases;
}
