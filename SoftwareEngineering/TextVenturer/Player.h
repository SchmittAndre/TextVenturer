#pragma once

class Item;

class Player
{
private:
    vector<Item*> items;

public:                   
	void addItem(Item* item);
	bool delItem(Item* item);

	bool hasItem(Item* item);

	vector<Item*> getInventory(); 
};

