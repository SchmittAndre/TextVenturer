#pragma once

class Item;
class Player;
class AdventureObject;

class Inventory
{
private:
    ref_optional<Player> player;
    ref_vector<Item> items;

    void loadItems(FileStream & stream, const ref_vector<AdventureObject> & objectList);

public:
    Inventory(FileStream & stream, const ref_vector<AdventureObject> & objectList, Player & player);
    Inventory(FileStream & stream, const ref_vector<AdventureObject> & objectList);
    Inventory(Player & player);
    Inventory();

	virtual void addItem(Item & item);
    void delItem(Item & item);
    bool hasItem(Item & item) const;

    Item& findItem(std::string name);

    bool isEmpty() const;    

    size_t getItemCount() const;
    std::string formatContents(Player* player = NULL) const;

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

