#pragma once
class AliasList
{
private:
    strings aliases;

public:
    AliasList(string name);

    bool add(string name);
    //bool del(string name);
    bool has(string name);

    string getName();
};

