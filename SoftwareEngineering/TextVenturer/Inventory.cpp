#include "stdafx.h"

#include "AliasList.h"
#include "CommandSystem.h"
#include "Item.h"
#include "Player.h"

#include "Inventory.h"  

void Inventory::loadItems(FileStream & stream, const ref_vector<AdventureObject> & objectList)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        items.push_back(dynamic_cast<Item&>(objectList.at(stream.readUInt()).get()));
}

Inventory::Inventory(FileStream & stream, const ref_vector<AdventureObject> & objectList, Player & player)
    : player(player)
{
    loadItems(stream, objectList);
}

Inventory::Inventory(FileStream & stream, const ref_vector<AdventureObject>& objectList)
{
    loadItems(stream, objectList);
}

Inventory::Inventory(Player & player)
    : player(player)
{
}

Inventory::Inventory()
{
}

void Inventory::addItem(Item & item)
{
    if (hasItem(item))
        throw(EAddItemExists, item);
    items.push_back(item);
    if (player)
        player->get().getCommandSystem().add(item.getCarryCommands());
}

Item& Inventory::findItem(std::string name)
{
    for (auto item = items.begin(); item != items.end(); item++)
        if (item->get().getAliases().has(name))
            return *item;
    throw(EItemNotFound, name);
}
            
bool Inventory::isEmpty() const
{
    return items.size() == 0;
}

void Inventory::delItem(Item& item)
{
    auto i = find(items.begin(), items.end(), item);
    if (i == items.end())
        throw(EDelItemMissing, item);
    items.erase(i);
    if (player)
        player->get().getCommandSystem().del(item.getCarryCommands());
}

bool Inventory::hasItem(Item & item) const
{
    return find(items.cbegin(), items.cend(), item) != items.cend();
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
    for (auto item = items.cbegin(); item != items.cend(); item++)
    {
        if (player)
            result += item->get().getName(player->knows(*item));
        else
            result += item->get().getName();
        if (items.size() > 2 && item < items.end() - 2)
            result += ", ";
        else if (items.size() > 1 && item != items.end() - 1)
            result += " and ";
    }
    return result;
}

void Inventory::save(FileStream & stream, const ref_idlist<AdventureObject> & objectIDs) const
{
    stream.write(static_cast<UINT>(items.size()));
    for (Item & item : items)
        stream.write(objectIDs.at(item));
}

EAddItemExists::EAddItemExists(Item * item)
    : Exception("Cannot add item \"" + item->getName() + "\" to inventory, as it exists already")
{
}

EDelItemMissing::EDelItemMissing(Item * item)
    : Exception("Cannot remove item \"" + item->getName() + "\" from inventory, as it doesn't exist")
{
}

EItemNotFound::EItemNotFound(std::string name)
    : Exception("Cannot find an item called \"" + name + "\" in inveentory")
{
}
