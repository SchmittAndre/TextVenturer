#include "stdafx.h"

#include "CommandSystem.h"
#include "Item.h"
#include "CustomAdventureAction.h"
#include "Adventure.h"

#include "ItemCombiner.h"

bool ItemCombiner::Entry::hasInput(const Item & item1, const Item & item2) const
{
    return this->item1 == &item1 && this->item2 == &item2 ||
           this->item1 == &item2 && this->item2 == &item1;
}

ItemCombiner::Entry::Entry(Item & item1, Item & item2, Item & result, CustomAdventureAction * onCombine)
    : item1(&item1)
    , item2(&item2)
    , result(&result)
    , onCombine(onCombine)
{
}

ItemCombiner::Entry::Entry(FileStream & stream, AdventureLoadHelp & help)
    : item1(&dynamic_cast<Item&>(help.objects[stream.readUInt()].get()))
    , item2(&dynamic_cast<Item&>(help.objects[stream.readUInt()].get()))
    , result(&dynamic_cast<Item&>(help.objects[stream.readUInt()].get()))
    , onCombine(CustomAdventureAction::loadConditional(stream, help.adventure))
{                              
}

void ItemCombiner::Entry::save(FileStream & stream, AdventureSaveHelp & help) const
{
    stream.write(help.objects[item1]);
    stream.write(help.objects[item2]);
    stream.write(help.objects[result]);
    CustomAdventureAction::saveConditional(stream, onCombine);
}

ItemCombiner::ItemCombiner()
{
}

ItemCombiner::~ItemCombiner()
{
    for (const Entry & entry : combinations)
        delete entry.onCombine;
}

void ItemCombiner::addCombination(Item & item1, Item & item2, Item & result, CustomAdventureAction * onCombine)
{
    for (auto & entry : combinations)
        if (entry.hasInput(item1, item2))
            throw(ECombinationExistsAlready, item1, item2, *entry.result);       
    combinations.push_back(Entry(item1, item2, result, onCombine));
}

void ItemCombiner::delCombination(Item & item1, Item & item2)
{
    auto pos = combinations.begin();
    for (; pos != combinations.end(); pos++)
        if (pos->hasInput(item1, item2))
            break;
    if (pos == combinations.end())
        throw(ECombinationDoesNotExists, item1, item2);
    combinations.erase(pos);
}

Item & ItemCombiner::getResult(Item & item1, Item & item2) const
{
    for (const Entry & entry : combinations)
        if (entry.hasInput(item1, item2))
            return *entry.result;
    throw(ECombinationDoesNotExists, item1, item2);
}

CustomAdventureAction* ItemCombiner::getOnCombine(Item & item1, Item & item2) const
{
    for (const Entry & entry : combinations)
        if (entry.hasInput(item1, item2))
            return entry.onCombine;
    throw(ECombinationDoesNotExists, item1, item2);
}

void ItemCombiner::save(FileStream & stream, AdventureSaveHelp & help) const
{
    stream.write(static_cast<UINT>(combinations.size()));
    for (const Entry & combination : combinations)
        combination.save(stream, help);
}

void ItemCombiner::load(FileStream & stream, AdventureLoadHelp & help)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        combinations.push_back(Entry(stream, help));
}

ECombinationExistsAlready::ECombinationExistsAlready(const Item & item1, const Item & item2, const Item & result)
    : Exception("The combination \"" + item1.getNameOnly() + " + " + item2.getNameOnly() + "\" exists already and gives \"" + result.getNameOnly() + "\"")
{
}

ECombinationDoesNotExists::ECombinationDoesNotExists(const Item & item1, const Item & item2)
    : Exception("The combination \"" + item1.getNameOnly() + " + " + item2.getNameOnly() + "\" does not exists")
{
}
