#pragma once

#include "Item.h"

class ItemCombiner
{
private:
    struct Entry {
        Item* item1;
        Item* item2;
        Item* result;

        Entry(Item* item1, Item* item2, Item* result);
    };

    std::vector<Entry> combinations;

public:          
    bool addCombination(Item* item1, Item* item2, Item* result);
    bool delCombination(Item* item1, Item* item2);

    Item* getResult(Item* item1, Item* item2) const;
};

