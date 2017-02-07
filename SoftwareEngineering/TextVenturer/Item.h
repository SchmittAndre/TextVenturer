#pragma once

class Player;

class Item
{
private:
    AliasList aliases;
    std::string description;

public:
    AliasList& getAliases();
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player* player, bool startOfSentence = false) const;
    bool isNamePlural() const;
    void setDescription(std::string description);
    std::string getDescription() const;
};