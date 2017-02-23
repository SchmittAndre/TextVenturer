#include "stdafx.h"

#include "CommandSystem.h"
#include "Item.h"
#include "Player.h"

#include "Inventory.h"  

Inventory::Inventory(Player * player)
{
    this->player = player;
}

bool Inventory::addItem(Item* item)
{
    items.push_back(item);
    if (player)
    {
        player->getCommandSystem()->add(item->getCarryCommands());
    }
    return true;
}

Item* Inventory::findItem(std::string name) const
{
    for (std::vector<Item*>::const_iterator item = items.begin(); item != items.end(); item++)
        if ((*item)->getAliases().has(name))
            return *item;
    return NULL;
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
        if (player)
        {
            player->getCommandSystem()->del(item->getCarryCommands());
        }
        return true;
    }
    return false;
}

bool Inventory::hasItem(Item * item) const
{
    return find(items.cbegin(), items.cend(), item) != items.cend();
}

std::vector<Item*> Inventory::getItems() const
{
	return items;
}

size_t Inventory::getItemCount() const
{
    return items.size();
}                 

std::string Inventory::formatContents(Player* player) const
{
    if (items.empty())
        return "nothing";
    std::string result = "";
    for (std::vector<Item*>::const_iterator item = items.begin(); item != items.end(); item++)
    {
        if (player)
            result += (*item)->getName(player->knows(*item));
        else
            result += (*item)->getName();
        if (items.size() > 2 && item < items.end() - 2)
            result += ", ";
        else if (items.size() > 1 && item != items.end() - 1)
            result += " and ";
    }
    return result;
}
