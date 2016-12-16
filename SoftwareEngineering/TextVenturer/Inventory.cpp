#include "stdafx.h"
#include "Item.h"

#include "Inventory.h"  

void Inventory::addItem(Item* item)
{
	items.push_back(item);
}

Item* Inventory::findItem(string name)
{
    for (vector<Item*>::iterator item = items.begin(); item != items.end(); item++)
        if ((*item)->getAliases()->has(name))
            return *item;
    return NULL;
}

void Inventory::clear()
{
    items.clear();
}

bool Inventory::isEmpty()
{
    return items.size() == 0;
}

bool Inventory::delItem(Item* item)
{
    vector<Item*>::iterator i = find(items.begin(), items.end(), item);
    if (i != items.end())
    {
        items.erase(i);
        return true;
    }
    return false;
}

vector<Item*> Inventory::getInventory()
{
	return items;
}
