#include "stdafx.h"

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

AliasList* Location::getAliases()
{
    return aliases;
}

string Location::getDescription()
{
    return description;
}

void Location::changeDescription(string description)
{
    this->description = description;
}
