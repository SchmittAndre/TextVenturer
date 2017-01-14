#pragma once

#include "Location.h"

class Inventory;

class Container : public Location
{
private:                         
    Inventory* inventory;

    bool accessible;
    string lockedDescription;

public:
    Container(string name, string description, bool accessible = true, string lockedDescription = "");
    ~Container();

    Inventory* getInventory() const;

    bool isAccessible() const;

    void lock();
    void unlock();

    string getDescription() const;
    void changeLockedDescription(string description);
};

