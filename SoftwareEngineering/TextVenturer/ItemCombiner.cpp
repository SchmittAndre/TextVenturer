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

void ItemCombiner::triggerEvent(Item * item1, Item * item2) const
{
    for (std::vector<Entry>::const_iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
        {
            if (entry->onCombine)
                entry->onCombine->run();
            break;
        }

}
