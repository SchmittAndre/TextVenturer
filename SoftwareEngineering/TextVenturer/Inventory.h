#pragma once

class Item;

class Inventory
{
private:
    vector<Item*> items;

public:                   
	void addItem(Item* item);
	bool delItem(Item* item);

    Item* findItem(string name);

    void clear();
    bool isEmpty();

	vector<Item*> getInventory(); 
};

