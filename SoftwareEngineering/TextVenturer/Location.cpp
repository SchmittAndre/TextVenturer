#include "stdafx.h"

#include "Location.h"

Location::Location(string name, string description)
{
    this->name = name;
    this->description = description;
}

string Location::getName()
{
    return name;
}

string Location::getDescription()
{
    return description;
}

void Location::changeDescription(string description)
{
    this->description = description;
}
