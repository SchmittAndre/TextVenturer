#pragma once

class Item;
class Player;

class Inventory
{
private:
    Player* player;
    std::vector<Item*> items;

public:             
    Inventory(FileStream & stream, std::vector<AdventureObject*> & objectList, Player* player = NULL);
    Inventory(Player* player = NULL);

	virtual void addItem(Item* item);
    void delItem(Item* item);
    bool hasItem(Item* item) const;

    Item* findItem(std::string name) const;

    bool isEmpty() const;    

	std::vector<Item*> getItems() const;
    size_t getItemCount() const;
    std::string formatContents(Player* player = NULL) const;

    virtual void save(FileStream & stream, idlist<AdventureObject*> & objectIDs) const;
};

class EAddItemExists : public Exception
{
public:
    EAddItemExists(Item* item);
};

class EDelItemMissing : public Exception
{
public:
    EDelItemMissing(Item* item);
};

class EItemNotFound : public Exception
{
public:
    EItemNotFound(std::string name);
};

