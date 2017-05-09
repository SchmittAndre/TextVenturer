#include "stdafx.h"

#include "CommandSystem.h"
#include "Command.h"
#include "CustomAdventureAction.h"
#include "Player.h"
#include "Item.h"
#include "Room.h"

#include "Location.h"

AdventureObject::Type Location::getType() const
{
    return otLocation;
}

Location::Location()
{
    onGoto = NULL;
    onLeave = NULL;
    locatedCommands = new CommandArray();
}

Location::~Location()
{
    delete locatedCommands;
    delete onGoto;
    delete onLeave;
}

Location::PInventory & Location::addInventory(std::string preposition)
{
    if (hasInventory(preposition))
        throw(ETodo);
    return inventories[preposition] = PInventory();
}

void Location::delInventory(std::string preposition)
{
    try
    {
        inventories.erase(preposition);
    }
    catch ()
    {
        throw(ETodo);
    }
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

Location::PInventory & Location::firstFilledInventory() const
{
    for (auto inv : inventories)
        if (!inv.second.isEmpty())
            return inv.second;
    throw(ETodo);
}
/*
std::vector<Location::PInventory*> Location::getInventories() const
{
    std::vector<PInventory*> invs;
    for (auto entry : inventories)
        invs.push_back(entry.second);
    return invs;
}
*/
CommandArray * Location::getLocatedCommands() const
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

Location::PInventory & Location::getInventory(std::string preposition)
{
    auto entry = inventories.find(preposition);
    if (entry == inventories.end())
        throw(EPrepositionNotFound, this, preposition);
    return entry->second;
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

void Location::save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs, ref_idlist<CommandArray> & commandArrayIDs) const
{
    AdventureObject::save(stream, objectIDs, commandArrayIDs);       
    stream.write(static_cast<UINT>(inventories.size()));
    for (auto entry : inventories)
    {
        stream.write(entry.first);
        entry.second.save(stream, objectIDs);
    }
    locatedCommands->save(stream);
    commandArrayIDs[locatedCommands] = static_cast<UINT>(commandArrayIDs.size());
}

void Location::load(FileStream & stream, Adventure & adventure, ref_vector<AdventureObject> & objectList, ref_vector<CommandArray> & commandArrayList)
{
    AdventureObject::load(stream, adventure, objectList, commandArrayList);
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
    {
        std::string prep = stream.readString();
        inventories[prep] = PInventory(stream, objectList);
    }
    locatedCommands->load(stream, adventure);
    commandArrayList.push_back(locatedCommands);
}

Location::PInventory::PInventory(FileStream & stream, ref_vector<AdventureObject> & objectList)
    : Inventory(stream, objectList)
{
    stream.read(prepAliasesList);
    stream.read(prepAliasesTake);
    if (stream.readBool())
    {
        filter = Inventory(stream, objectList);
        mode = static_cast<Filter>(stream.readByte());
    }
}

Location::PInventory::PInventory()
{
    mode = ifBlacklist;
}

bool Location::PInventory::addPrepositionAlias(std::string alias, bool runOnTake)
{
    if (hasPrepositionAlias(alias, true))
        return false;

    if (runOnTake)
        prepAliasesTake.push_back(alias);
    else
        prepAliasesList.push_back(alias);

    return true;
}

bool Location::PInventory::delPrepositionAlias(std::string alias)
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

std::string Location::PInventory::getPrepositionName(bool runOnTake, bool startOfSentence) const
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

bool Location::PInventory::hasPrepositionAlias(std::string alias, bool runOnTake) const
{
    return find(prepAliasesList.begin(), prepAliasesList.end(), alias) != prepAliasesList.end() ||
           runOnTake && find(prepAliasesTake.begin(), prepAliasesTake.end(), alias) != prepAliasesTake.end();
}

void Location::PInventory::addItem(Item & item)
{                           
    if (!canAddItem(item))
        throw(EAddItemFilterForbidden, item);
    Inventory::addItem(item);
}

bool Location::PInventory::canAddItem(Item & item) const
{
    return !filter || (mode == PInventory::ifBlacklist) ^ filter->hasItem(item);
}

void Location::PInventory::addItemForce(Item & item)
{
    Inventory::addItem(item);
}

bool Location::PInventory::isFiltered() const
{
    return filter.has_value();
}

Location::PInventory::Filter Location::PInventory::getFilterMode() const
{
    return mode;
}

void Location::PInventory::enableFilter(Filter mode)
{
    this->mode = mode;
    filter = Inventory();
}

void Location::PInventory::disableFilter()
{
    filter.reset();
}

void Location::PInventory::addToFilter(Item & item)
{
    filter->addItem(item);
}

void Location::PInventory::delFromFilter(Item & item)
{
    filter->delItem(item);
}

void Location::PInventory::save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs) const
{
    Inventory::save(stream, objectIDs);
    stream.write(prepAliasesList);             
    stream.write(prepAliasesTake);    
    stream.write(isFiltered());
    if (isFiltered())
    {
        filter->save(stream, objectIDs);
        stream.write(static_cast<byte>(mode));
    }
}

EPrepositionNotFound::EPrepositionNotFound(const Location* location, std::string preposition)
    : Exception("Preposition \"" + preposition + "\" not found in location \"" + location->getName() + "\"")
{
}

EAddItemFilterForbidden::EAddItemFilterForbidden(Item * item)
    : Exception("Cannot add item \"" + item->getName() + "\" to the preposition-inventory, as the filter does not allow it")
{
}
