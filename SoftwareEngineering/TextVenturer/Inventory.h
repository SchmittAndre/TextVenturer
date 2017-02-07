#pragma once

class Item;
class Player;

class Inventory
{
private:
    std::vector<Item*> items;

public:                   
	virtual bool addItem(Item* item);
    bool delItem(Item* item);
    bool hasItem(Item* item) const;

    Item* findItem(std::string name) const;

    void clear();
    bool isEmpty() const;    

	std::vector<Item*> getItems() const;
    size_t getItemCount() const;
    std::string formatContents(Player* player = NULL) const;
};

