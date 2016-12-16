#pragma once

class Item
{
private:
    strings aliases;
    string description;

public:
    Item(string name, string description);

    bool addAlias(string name);
    bool delAlias(string name);
    bool hasAlias(string name);

    string getName();
    strings getAliases();
    string getDescription();
};