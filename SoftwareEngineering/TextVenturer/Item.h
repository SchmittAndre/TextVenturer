#pragma once

class Item
{
private:
    AliasList aliases;
    std::string description;

public:
    AliasList& getAliases();
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    bool isNamePlural() const;
    void setDescription(std::string description);
    std::string getDescription() const;
};