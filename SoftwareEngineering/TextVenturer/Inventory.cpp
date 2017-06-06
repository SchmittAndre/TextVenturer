#include "stdafx.h"

#include "Adventure.h"
#include "AliasList.h"
#include "CommandSystem.h"
#include "Player.h"
#include "Item.h"

#include "Inventory.h"  

Inventory::Inventory(FileStream & stream, AdventureLoadHelp & help)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        items.push_back(dynamic_cast<Item&>(help.objects[stream.readUInt()].get()));
}

Inventory::Inventory()
{
}

void Inventory::addItem(Item & item)
{
    if (hasItem(item))
        throw(EAddItemExists, item);
    items.push_back(item);
}

Item & Inventory::findItem(std::string name) const
{
    auto pos = std::find_if(items.begin(), items.end(), [&](Item & a) 
    {
        return a.getAliases().has(name);
    });
    if (pos == items.end())
        throw(EItemNotFound, name);
    return *pos;
}
            
bool Inventory::isEmpty() const
{
    return items.size() == 0;
}

void Inventory::delItem(Item & item)
{
    auto pos = std::find_if(items.begin(), items.end(), [&](Item & a) 
    {
        return &a == &item;
    });
    if (pos == items.end())
        throw(EItemDoesNotExist, item);
    items.erase(pos);
}

bool Inventory::hasItem(Item & item) const
{
    return std::find_if(items.begin(), items.end(), [&](Item & a)
    {
        return &a == &item;
    }) != items.cend();
}

const ref_vector<Item> & Inventory::getItems() const
{
    return items;
}

void Inventory::delAll()
{
    items.clear();
}

size_t Inventory::getItemCount() const
{
    return items.size();
}

std::string Inventory::formatContents(Player & player, bool startOfSentence) const
{
    if (items.empty())
        return "nothing";
    std::string result = "";
    for (auto item = items.cbegin(); item != items.cend(); item++)
    {
        result += item->get().getName(player);
        if (items.size() > 2 && item < items.end() - 2)
            result += ", ";
        else if (items.size() > 1 && item != items.end() - 1)
            result += " and ";
    }
    if (startOfSentence && result.size() > 0)
        result[0] = toupper(result[0]);
    return result;
}             

void Inventory::save(FileStream & stream, AdventureSaveHelp & help) const
{
    stream.write(static_cast<UINT>(items.size()));
    for (Item & item : items)
        stream.write(help.objects[&item]);
}

EAddItemExists::EAddItemExists(Item & item)
    : Exception("Cannot add item \"" + item.getNameOnly() + "\" to inventory, as it exists already")
{
}

EItemDoesNotExist::EItemDoesNotExist(Item & item)
    : Exception("Cannot remove item \"" + item.getNameOnly() + "\" from inventory, as it doesn't exist")
{
}

EItemNotFound::EItemNotFound(std::string name)
    : Exception("Cannot find an item called \"" + name + "\" in inveentory")
{
}
