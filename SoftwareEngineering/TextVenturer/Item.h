#pragma once

class Item
{
private:
    AliasList* aliases;
    std::string description;

public:
    Item(std::string name, std::string description);
    ~Item();

    AliasList* getAliases() const;
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    bool isNamePlural() const;
    std::string getDescription() const;
};