#pragma once
class Player
{
private:
	vector<string> items;

public:
	Player();
	~Player();
	void addItem(string a);

	bool checkItem(string pstring);

	bool deleteItem(string a);

	vector<string> getInventory();

};

