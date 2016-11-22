#pragma once
class Player
{
private:
	vector<string> items;

public:
	Player();
	~Player();
	void additem(string a);

	vector<string> getInventory();

};

