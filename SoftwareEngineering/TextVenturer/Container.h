#pragma once

#include "Location.h"

class Inventory;

class Container : public Location
{
private:
    Inventory* inventory;
public:
    Container(string name, string description);
    ~Container();

    Inventory* getInventory();
};

