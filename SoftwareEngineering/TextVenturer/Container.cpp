#include "stdafx.h"
#include "Inventory.h"

#include "Container.h"

Container::Container(string name, string description)
:   Location(name, description)
{
    inventory = new Inventory();
}

Container::~Container()
{
    delete inventory;
}

Inventory* Container::getInventory()
{
    return inventory;
}
