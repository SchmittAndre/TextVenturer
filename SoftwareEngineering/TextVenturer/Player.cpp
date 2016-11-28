#include "stdafx.h"

#include "Player.h"



Player::Player()
{
}


Player::~Player()
{
}

void Player::additem(string a)
{
	items.push_back(a);
}
vector<string> Player::getInventory()
{

	return items;
}
