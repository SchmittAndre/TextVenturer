#pragma once

class Item
{
private:
    // aliases[0] is the main name
    strings aliases;

public:
    Item(string name);

    bool addAlias(string name);
    bool delAlias(string name);

    string getName();
    strings getAliases();
};