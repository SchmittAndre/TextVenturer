#pragma once

#include "Item.h"

class Inventory
{
private:
    std::vector<Item*> items;

public:                   
	void addItem(Item* item);
	bool delItem(Item* item);

    Item* findItem(std::string name) const;

    void clear();
    bool isEmpty() const;


	std::vector<Item*> getItems() const;
    size_t getItemCount() const;
    std::string formatContents() const;
};

