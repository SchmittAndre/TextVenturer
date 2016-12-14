#include "stdafx.h"

#include "Player.h"



Player::Player()
{
}


Player::~Player()
{
}

void Player::addItem(string a)
{
	items.push_back(a);
}
bool Player::checkItem(string pstring)
{
	for (int i = 0; i<items.size(); i++)
	{
		if (items.at(i) == pstring)
		{
			return true;
		}
	}
	return false;
}
bool Player::deleteItem(string a)
{
	for (int i=0; i<items.size();i++) 
	{
		if (items[i] == a)
		{
			items.erase(items.begin()+i);
			return true;
		}
	}
	return false;
}

vector<string> Player::getInventory()
{
	return items;
}
