#pragma once

struct AdventureLoadHelp;
struct AdventureSaveHelp;
class Item;
class CustomAdventureAction;

class ItemCombiner
{
private:
    struct Entry {
        Item & item1;
        Item & item2;
        Item & result;

        CustomAdventureAction * onCombine;

        bool hasInput(const Item & item1, const Item & item2) const;

        Entry(Item & item1, Item & item2, Item & result, CustomAdventureAction * onCombine = NULL);
        Entry(FileStream & stream, AdventureLoadHelp & help);

        void save(FileStream & stream, AdventureSaveHelp & help);
    };

    std::vector<Entry> combinations;

public:             
    ItemCombiner();
    ItemCombiner(FileStream & stream, AdventureLoadHelp & help);

    ~ItemCombiner();

    void addCombination(Item & item1, Item & item2, Item & result, CustomAdventureAction * onCombine = NULL);
    void delCombination(Item & item1, Item & item2);

    Item & getResult(Item & item1, Item & item2) const;

    CustomAdventureAction* getOnCombine(Item & item1, Item & item2) const;

    void save(FileStream & stream, AdventureSaveHelp & help) const;
};

class ECombinationExistsAlready : public Exception
{
public:
    ECombinationExistsAlready(const Item & item1, const Item & item2, const Item & result);
};   

class ECombinationDoesNotExists : public Exception
{
public:
    ECombinationDoesNotExists(const Item & item1, const Item & item2);
};