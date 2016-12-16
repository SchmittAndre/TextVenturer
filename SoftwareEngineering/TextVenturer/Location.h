#pragma once

class Location
{
private:
    string name;
    string description;

public:
    Location(string name, string description);

    string getName();
    virtual string getDescription();

    void changeDescription(string description);
};

