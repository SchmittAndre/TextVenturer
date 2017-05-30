#include "stdafx.h"

#include "CommandSystem.h"
#include "Command.h"
#include "CustomAdventureAction.h"
#include "Player.h"
#include "Item.h"
#include "Room.h"
#include "Adventure.h"

#include "Location.h"

AdventureObject::Type Location::getType() const
{
    return otLocation;
}

Location::Location()
    : onGoto(NULL)
    , onLeave(NULL)
{
}

Location::Location(FileStream & stream, AdventureLoadHelp & help)
    : AdventureObject(stream, help)
    , locatedCommands(stream, help)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
    {
        std::string prep = stream.readString();
        inventories[prep] = MultiInventory(stream, help);
    }
    help.commandArrays.push_back(locatedCommands);
}

Location::~Location()
{
    delete onGoto;
    delete onLeave;
}

Location::MultiInventory & Location::addInventory(std::string preposition)
{
    if (hasInventory(preposition))
        throw(EPrepositionExistsAlready, *this, preposition);
    return inventories[preposition] = MultiInventory();
}

void Location::delInventory(std::string preposition)
{
    auto pos = inventories.find(preposition);
    if (pos == inventories.end())
        throw(EPrepositionNotFound, *this, preposition);
    inventories.erase(pos);
}

bool Location::hasInventory(std::string preposition) const
{
    return inventories.find(preposition) != inventories.end();
}

size_t Location::filledInventoryCount() const
{
    size_t result = 0;
    for (auto inv : inventories)
        if (!inv.second.isEmpty())
            result++;
    return result;
}

Location::MultiInventory & Location::firstFilledInventory() const
{
    for (auto inv : inventories)
        if (!inv.second.isEmpty())
            return inv.second;
    throw(EMultiInventoryEmpty, *this);
}

CommandArray & Location::getLocatedCommands()
{
    return locatedCommands;
}

CustomAdventureAction* Location::getOnGoto() const
{
    return onGoto;
}

CustomAdventureAction* Location::getOnLeave() const
{
    return onLeave;
}

void Location::setOnGoto(CustomAdventureAction * onGoto)
{
    this->onGoto = onGoto;
}

void Location::setOnLeave(CustomAdventureAction * onLeave)
{
    this->onLeave = onLeave;
}

Location::MultiInventory & Location::getInventory(std::string preposition)
{
    auto entry = inventories.find(preposition);
    if (entry == inventories.end())
        throw(EPrepositionNotFound, *this, preposition);
    return entry->second;
}

ref_vector<Location::MultiInventory> Location::getInventories() const
{
    ref_vector<MultiInventory> result;
    for (auto pair : inventories)
        result.push_back(pair.second);
    return  result;
}

std::string Location::formatPrepositions(bool filledOnly) const
{
    if (inventories.empty())
        return "none";
    std::string result = "";
    size_t i = 0;
    if (filledOnly)
    {
        for (auto inventory = inventories.begin(); i < filledInventoryCount(); inventory++, i++)
        {
            if (inventory->second.isEmpty())
                continue;
            if (i + 1 == filledInventoryCount() && result != "")
                result += " and ";
            result += inventory->second.getPrepositionName();
            if (i + 2 < filledInventoryCount())
                result += ", ";
        }
    }
    else
    {
        for (auto inventory = inventories.begin(); i < inventories.size(); inventory++, i++)
        {
            if (i + 1 == inventories.size() && result != "")
                result += " and ";
            result += inventory->second.getPrepositionName();
            if (i + 2 < inventories.size())
                result += ", ";
        }
    }
    return result;
}

std::string Location::formatPrepositions(Item & filterCheckItem) const
{
    if (inventories.empty())
        return "none";
    std::string result = "";
    size_t i = 0;
        for (auto inventory = inventories.begin(); i < filledInventoryCount(); inventory++, i++)
        {
            if (!inventory->second.canAddItem(filterCheckItem))
                continue;
            if (i + 1 == filledInventoryCount() && result != "")
                result += " and ";
            result += inventory->second.getPrepositionName();
            if (i + 2 < filledInventoryCount())
                result += ", ";
        }
  
    return result;
}

void Location::save(FileStream & stream, AdventureSaveHelp & help) const
{
    AdventureObject::save(stream, help);       
    stream.write(static_cast<UINT>(inventories.size()));
    for (auto entry : inventories)
    {
        stream.write(entry.first);
        entry.second.save(stream, help);
    }
    locatedCommands.save(stream, help);
    help.commandArrays[&locatedCommands] = static_cast<UINT>(help.commandArrays.size());
}

Location::MultiInventory::MultiInventory(FileStream & stream, AdventureLoadHelp & help)
    : Inventory(stream, help)
    , prepAliasesList(stream.readStrings())
    , prepAliasesTake(stream.readStrings())
    , mode(static_cast<Filter>(stream.readByte()))
    , filter(stream, help)
{
}

Location::MultiInventory::MultiInventory()
{
    mode = ifBlacklist;
}

bool Location::MultiInventory::addPrepositionAlias(std::string alias, bool runOnTake)
{
    if (hasPrepositionAlias(alias, true))
        return false;

    if (runOnTake)
        prepAliasesTake.push_back(alias);
    else
        prepAliasesList.push_back(alias);

    return true;
}

bool Location::MultiInventory::delPrepositionAlias(std::string alias)
{
    stringlist::iterator entry;
    entry = find(prepAliasesList.begin(), prepAliasesList.end(), alias);
    if (entry != prepAliasesList.end())
    {
        prepAliasesList.erase(entry);
        return true;
    }
    entry = find(prepAliasesTake.begin(), prepAliasesTake.end(), alias);
    if (entry != prepAliasesTake.end())
    {
        prepAliasesTake.erase(entry);
        return true;
    }                   
    return false;
}

std::string Location::MultiInventory::getPrepositionName(bool runOnTake, bool startOfSentence) const
{
    std::string name;
    if (runOnTake && prepAliasesTake.size() > 0)
        name = prepAliasesTake[0];
    else if (prepAliasesList.size() > 0)
        name = prepAliasesList[0];
    else
        name = "[missing preposition]";
    if (startOfSentence)
        name[0] = toupper(name[0]);
    return name;
}

bool Location::MultiInventory::hasPrepositionAlias(std::string alias, bool runOnTake) const
{
    return find(prepAliasesList.begin(), prepAliasesList.end(), alias) != prepAliasesList.end() ||
           runOnTake && find(prepAliasesTake.begin(), prepAliasesTake.end(), alias) != prepAliasesTake.end();
}

void Location::MultiInventory::addItem(Item & item)
{                           
    if (!canAddItem(item))
        throw(EAddItemFilterForbidden, item);
    Inventory::addItem(item);
}

bool Location::MultiInventory::canAddItem(Item & item) const
{
    return (mode == MultiInventory::ifBlacklist) != filter.hasItem(item);
}

void Location::MultiInventory::addItemForce(Item & item)
{
    Inventory::addItem(item);
}

Location::MultiInventory::Filter Location::MultiInventory::getFilterMode() const
{
    return mode;
}

void Location::MultiInventory::enableFilter(Filter mode)
{
    this->mode = mode;
    filter = Inventory();
}

void Location::MultiInventory::disableFilter()
{
    mode = ifBlacklist;
    filter.delAll();
}

void Location::MultiInventory::addToFilter(Item & item)
{
    filter.addItem(item);
}

void Location::MultiInventory::delFromFilter(Item & item)
{
    filter.delItem(item);
}

void Location::MultiInventory::save(FileStream & stream, AdventureSaveHelp & help) const
{
    Inventory::save(stream, help);
    stream.write(prepAliasesList);             
    stream.write(prepAliasesTake);
    stream.write(static_cast<byte>(mode));
    filter.save(stream, help);
}

EPrepositionExistsAlready::EPrepositionExistsAlready(const Location & location, const std::string & preposition)
    : Exception("Preposition \"" + preposition + "\" exists already in location \"" + location.getName() + "\"")
{
}

EPrepositionNotFound::EPrepositionNotFound(const Location & location, const std::string & preposition)
    : Exception("Preposition \"" + preposition + "\" not found in location \"" + location.getName() + "\"")
{
}

EAddItemFilterForbidden::EAddItemFilterForbidden(const Item & item)
    : Exception("Cannot add item \"" + item.getName() + "\" to the preposition-inventory, as the filter does not allow it")
{
}

EMultiInventoryEmpty::EMultiInventoryEmpty(const Location & location)
    : Exception("Location \"" + location.getName() + "\" does not contain any inventories")
{
}
