#include "stdafx.h"
#include "Item.h"

#include "ItemCombiner.h"

ItemCombiner::Entry::Entry(Item* item1, Item* item2, Item* result)
{
    this->item1 = item1;
    this->item2 = item2;
    this->result = result;
}

bool ItemCombiner::addCombination(Item* item1, Item* item2, Item* result)
{
    for (vector<Entry>::iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
            return false;       
    combinations.push_back(Entry(item1, item2, result));
    return true;    
}

bool ItemCombiner::delCombination(Item* item1, Item* item2)
{
    for (vector<Entry>::iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
        {
            combinations.erase(entry);
            return true;
        }
    return false;
}

Item* ItemCombiner::getResult(Item* item1, Item* item2)
{
    for (vector<Entry>::iterator entry = combinations.begin(); entry != combinations.end(); entry++)
        if (entry->item1 == item1 && entry->item2 == item2 ||
            entry->item1 == item2 && entry->item2 == item1)
            return entry->result;
    return NULL;
}
