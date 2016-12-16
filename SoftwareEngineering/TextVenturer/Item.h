#pragma once

class AliasList;

class Item
{
private:
    AliasList* aliases;
    string description;

public:
    Item(string name, string description);
    ~Item();

    AliasList* getAliases();
    string getDescription();
};