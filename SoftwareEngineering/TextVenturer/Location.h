#pragma once

class AliasList;

class Location
{
private:
    AliasList* aliases;
    string description;

public:
    Location(string name, string description);
    ~Location();

    AliasList* getAliases();
    virtual string getDescription();

    void changeDescription(string description);
};

