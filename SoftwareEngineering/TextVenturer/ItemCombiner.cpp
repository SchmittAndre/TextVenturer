#include "stdafx.h"

#include "CommandSystem.h"
#include "Item.h"
#include "CustomAdventureAction.h"

#include "ItemCombiner.h"

ItemCombiner::Entry::Entry(Item* item1, Item* item2, Item* result, CustomAdventureAction* onCombine)
{
    this->item1 = item1;
    this->item2 = item2;
    this->result = result;
    this->onCombine = onCombine;
}

ItemCombiner::~ItemCombiner()
{
    for (Entry entry : combinations)
        delete entry.onCombine;
}

bool ItemCombiner::addCombination(Item* item1, Item* item2, Item* result, CustomAdventureAction* onCombine)
{
    for (std::vector<Entry>::iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
            return false;       
    combinations.push_back(Entry(item1, item2, result, onCombine));
    return true;    
}

bool ItemCombiner::delCombination(Item* item1, Item* item2)
{
    for (std::vector<Entry>::iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
        {
            combinations.erase(entry);
            return true;
        }
    return false;
}

Item* ItemCombiner::getResult(Item* item1, Item* item2) const
{
    for (std::vector<Entry>::const_iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
            return entry->result;                  
    return NULL;
}

CustomAdventureAction* ItemCombiner::getOnCombine(Item * item1, Item * item2) const
{
    for (std::vector<Entry>::const_iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
        {
            if (entry->onCombine)
                return entry->onCombine;
            break;
        }
    return NULL;
}

void ItemCombiner::save(FileStream & stream, idlist<AdventureObject*> objectIDs)
{
    stream.write(static_cast<UINT>(combinations.size()));
    for (Entry combination : combinations)
    {
        stream.write(objectIDs[combination.item1]);
        stream.write(objectIDs[combination.item2]);
        stream.write(objectIDs[combination.result]);
        stream.write(combination.onCombine != NULL);
        if (combination.onCombine)
            combination.onCombine->save(stream);
    }
}
