#include "stdafx.h"

#include "AliasList.h"

AliasList::AliasList(string name)
{
    aliases.push_back(name);
}

bool AliasList::add(string name)
{
    if (find(aliases.begin(), aliases.end(), name) != aliases.end())
        return false;
    aliases.push_back(name);
    return true;
}
/*
bool AliasList::del(string name)
{
    strings::iterator i = find(aliases.begin(), aliases.end(), name);
    if (i == aliases.end())
        return false;
    aliases.erase(i);
    return true;
}
*/
bool AliasList::has(string name)
{
    return find(aliases.begin(), aliases.end(), name) != aliases.end();
}

string AliasList::getName()
{
    return aliases[0];
}
