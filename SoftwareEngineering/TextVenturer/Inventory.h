#pragma once

class Item;
class Player;
class AdventureObject;

class Inventory
{
private:
    ref_vector<Item> items;

    void loadItems(FileStream & stream, const ref_vector<AdventureObject> & objectList);

public:
    Inventory(FileStream & stream, const ref_vector<AdventureObject> & objectList);
    Inventory();

	virtual void addItem(Item & item);
    virtual void delItem(Item & item);
    bool hasItem(Item & item) const;

    void delAll();

    Item & findItem(std::string name);

    bool isEmpty() const;    

    size_t getItemCount() const;
    
    std::string formatContents(Player & player, bool startOfSentence = false) const;
    
    virtual void save(FileStream & stream, const ref_idlist<AdventureObject> & objectIDs) const;

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

