#pragma once

class Item;

class Inventory
{
private:
    vector<Item*> items;

public:                   
	void addItem(Item* item);
	bool delItem(Item* item);

    Item* findItem(string name) const;

    void clear();
    bool isEmpty() const;

	vector<Item*> getItems() const;
    size_t getItemCount() const;
    string formatContents() const;
};

