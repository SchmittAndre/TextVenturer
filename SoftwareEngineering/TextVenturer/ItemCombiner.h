#pragma once
class ItemCombiner
{
public:
    struct Entry {
        Item* item1;
        Item* item2;
        Item* result;

        Entry(Item* item1, Item* item2, Item* result);
    };

private:
    vector<Entry> combinations;

public:          
    bool addCombination(Item* item1, Item* item2, Item* result);
    bool delCombination(Item* item1, Item* item2);

    Item* getResult(Item* item1, Item* item2);   
};

