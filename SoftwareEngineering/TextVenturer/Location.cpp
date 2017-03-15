#include "stdafx.h"

#include "CommandSystem.h"
#include "Command.h"
#include "CustomAdventureAction.h"
#include "Player.h"
#include "Item.h"
#include "Room.h"

#include "Location.h"

Location::LocatedCommandAction::LocatedCommandAction(Command * command, CustomAdventureAction * action, bool anywhere)
:   commandAction(command, action)
{
    this->anywhere = anywhere;
}  

Location::Location()
{
    onGoto = NULL;
    onLeave = NULL;
    locatedCommands = new CommandArray();
}

Location::~Location()
{
    for (auto entry : inventories)
        delete entry.second;
    delete locatedCommands;
    delete onGoto;
    delete onLeave;
}

void Location::addCommand(Command * command, CustomAdventureAction * action, bool anywhere)
{
    commands.push_back(LocatedCommandAction(command, action, anywhere));
}

Location::PInventory* Location::addInventory(std::string preposition)
{
    if (hasInventory(preposition))
        return NULL;
    return inventories[preposition] = new PInventory();
}

bool Location::delInventory(std::string preposition)
{
    auto entry = inventories.find(preposition);
    if (entry == inventories.end())
        return false;

    delete entry->second;
    inventories.erase(entry);
    return true;
}

bool Location::hasInventory(std::string preposition)
{
    return inventories.find(preposition) != inventories.end();
}

size_t Location::filledInventoryCount()
{
    size_t result = 0;
    for (auto inv : inventories)
        result += (int)!inv.second->isEmpty();
    return result;
}

Location::PInventory * Location::firstFilledInventory()
{
    for (auto inv : inventories)
        if (!inv.second->isEmpty())
            return inv.second;
    return NULL;
}

std::vector<Location::PInventory*> Location::getInventories()
{
    std::vector<PInventory*> invs;
    for (auto entry : inventories)
        invs.push_back(entry.second);
    return invs;
}

CommandArray * Location::getLocatedCommands()
{
    return locatedCommands;
}

CustomAdventureAction* Location::getOnGoto()
{
    return onGoto;
}

CustomAdventureAction* Location::getOnLeave()
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

Location::PInventory * Location::getInventory(std::string preposition)
{
    auto entry = inventories.find(preposition);
    return entry != inventories.end() ? entry->second : NULL;
}

std::string Location::formatPrepositions(bool filledOnly)
{
    if (inventories.empty())
        return "none";
    std::string result = "";
    size_t i = 0;
    if (filledOnly)
    {
        for (auto inventory = inventories.begin(); i < filledInventoryCount(); inventory++, i++)
        {
            if (inventory->second->isEmpty())
                continue;
            if (i + 1 == filledInventoryCount() && result != "")
                result += " and ";
            result += (*inventory).second->getPrepositionName();
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
            result += (*inventory).second->getPrepositionName();
            if (i + 2 < inventories.size())
                result += ", ";
        }
    }
    return result;
}

std::string Location::formatPrepositions(Item * filterCheckItem)
{
    if (inventories.empty())
        return "none";
    std::string result = "";
    size_t i = 0;
        for (auto inventory = inventories.begin(); i < filledInventoryCount(); inventory++, i++)
        {
            if (!inventory->second->canAddItem(filterCheckItem))
                continue;
            if (i + 1 == filledInventoryCount() && result != "")
                result += " and ";
            result += (*inventory).second->getPrepositionName();
            if (i + 2 < filledInventoryCount())
                result += ", ";
        }
  
    return result;
}

Location::PInventory::PInventory()
{
    filter = NULL;
    mode = ifBlacklist;
}

Location::PInventory::~PInventory()
{
    delete filter;
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
    strings::iterator entry;
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

bool Location::PInventory::addItem(Item * item)
{                                             
    return canAddItem(item) ? Inventory::addItem(item) : false;
}

bool Location::PInventory::canAddItem(Item * item)
{
    return !filter || (mode == PInventory::ifBlacklist) ^ filter->hasItem(item);
}

void Location::PInventory::addItemForce(Item * item)
{
    Inventory::addItem(item);
}

bool Location::PInventory::isFiltered() const
{
    return filter != NULL;
}

Location::PInventory::Filter Location::PInventory::getFilterMode() const
{
    return mode;
}

void Location::PInventory::enableFilter(Filter mode)
{
    this->mode = mode;
    delete filter;
    filter = new Inventory();
}

void Location::PInventory::disableFilter()
{
    delete filter;
    filter = NULL;
}

void Location::PInventory::addToFilter(Item * item)
{
    filter->addItem(item);
}

bool Location::PInventory::delFromFilter(Item * item)
{
    return filter->delItem(item);
}
