#pragma once

class Item;
class CustomAdventureAction;

class ItemCombiner
{
private:
    struct Entry {
        Item* item1;
        Item* item2;
        Item* result;
        CustomAdventureAction* onCombine;

        Entry(Item* item1, Item* item2, Item* result, CustomAdventureAction* onCombine = NULL);
    };

    std::vector<Entry> combinations;

public:             
    ~ItemCombiner();

    bool addCombination(Item* item1, Item* item2, Item* result, CustomAdventureAction* onCombine = NULL);
    bool delCombination(Item* item1, Item* item2);

    Item* getResult(Item* item1, Item* item2) const;

    CustomAdventureAction* getOnCombine(Item* item1, Item* item2) const;

    void save(FileStream & stream, idlist<AdventureObject*> objectIDs);
};

