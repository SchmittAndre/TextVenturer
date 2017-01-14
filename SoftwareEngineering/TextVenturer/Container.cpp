#include "stdafx.h"
#include "Inventory.h"

#include "Container.h"

Container::Container(string name, string description, bool accessible, string lockedDescription)
:   Location(name, description)
{
    inventory = new Inventory();
    this->accessible = accessible;
    this->lockedDescription = lockedDescription;
}

Container::~Container()
{
    delete inventory;
}

Inventory* Container::getInventory() const
{
    return inventory;
}

bool Container::isAccessible() const
{
    return accessible;
}

void Container::lock()
{
    accessible = false;
}

void Container::unlock()
{
    accessible = true;
}

string Container::getDescription() const
{
    return accessible ? Location::getDescription() : lockedDescription;
}

void Container::changeLockedDescription(string description)
{
    lockedDescription = description;
}
