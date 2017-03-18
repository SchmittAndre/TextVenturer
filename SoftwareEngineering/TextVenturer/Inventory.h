#pragma once

class Item;
class Player;

class Inventory
{
private:
    Player* player;
    std::vector<Item*> items;

public:                   
    Inventory(Player* player = NULL);

	virtual bool addItem(Item* item);
    bool delItem(Item* item);
    bool hasItem(Item* item) const;

    Item* findItem(std::string name) const;

    bool isEmpty() const;    

	std::vector<Item*> getItems() const;
    size_t getItemCount() const;
    std::string formatContents(Player* player = NULL) const;

    virtual void save(FileStream & stream, idlist<AdventureObject*> & objectIDs);
};

