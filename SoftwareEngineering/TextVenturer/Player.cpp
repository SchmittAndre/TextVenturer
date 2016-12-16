#include "stdafx.h"
#include "Item.h"

#include "Player.h"  

void Player::addItem(Item* item)
{
	items.push_back(item);
}
bool Player::hasItem(Item* item)
{
    return find(items.begin(), items.end(), item) != items.end();	        	    
}

bool Player::delItem(Item* item)
{
    vector<Item*>::iterator i = find(items.begin(), items.end(), item);
    if (i != items.end())
    {
        items.erase(i);
        return true;
    }
    return false;
}

vector<Item*> Player::getInventory()
{
	return items;
}
