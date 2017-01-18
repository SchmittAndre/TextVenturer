#pragma once

class Item
{
private:
    AliasList* aliases;
    string description;

public:
    Item(string name, string description);
    ~Item();

    AliasList* getAliases() const;
    string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    bool isNamePlural() const;
    string getDescription() const;
};