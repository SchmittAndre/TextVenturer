#include "stdafx.h"

#include "Inventory.h"  

void Inventory::addItem(Item* item)
{
	items.push_back(item);
}

Item* Inventory::findItem(std::string name) const
{
    for (std::vector<Item*>::const_iterator item = items.begin(); item != items.end(); item++)
        if ((*item)->getAliases()->has(name))
            return *item;
    return NULL;
}

void Inventory::clear()
{
    items.clear();
}

bool Inventory::isEmpty() const
{
    return items.size() == 0;
}

bool Inventory::delItem(Item* item)
{
    std::vector<Item*>::iterator i = find(items.begin(), items.end(), item);
    if (i != items.end())
    {
        items.erase(i);
        return true;
    }
    return false;
}

std::vector<Item*> Inventory::getItems() const
{
	return items;
}

size_t Inventory::getItemCount() const
{
    return items.size();
}


std::string Inventory::formatContents() const
{
    if (items.empty())
        return "nothing";
    std::string result = "";
    for (std::vector<Item*>::const_iterator item = items.begin(); item != items.end() - 1; item++)
    {
        result += (*item)->getName();
        if (item != items.end() - 2)
            result += ", ";
    }
    if (result != "")
        result += " and ";
    result += (*(items.end() - 1))->getName();
    return result;
}
