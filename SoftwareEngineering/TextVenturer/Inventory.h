#pragma once

struct AdventureLoadHelp;
struct AdventureSaveHelp;
class Item;
class Player;
class AdventureObject;

class Inventory
{
private:
    ref_vector<Item> items;

public:
    Inventory(FileStream & stream, AdventureLoadHelp & help);
    Inventory();

	virtual void addItem(Item & item);
    virtual void delItem(Item & item);
    bool hasItem(Item & item) const;

    ref_vector<Item> & getItems();

    void delAll();

    Item & findItem(std::string name);

    bool isEmpty() const;    

    size_t getItemCount() const;
    
    std::string formatContents(Player & player, bool startOfSentence = false) const;
    
    virtual void save(FileStream & stream, AdventureSaveHelp & help) const;

    // TODO: cbegin and cend iterators
};

class EAddItemExists : public Exception
{
public:
    EAddItemExists(Item & item);
};

class EDelItemMissing : public Exception
{
public:
    EDelItemMissing(Item & item);
};

class EItemNotFound : public Exception
{
public:
    EItemNotFound(std::string name);
};

