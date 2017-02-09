#pragma once

class Player;

class AdventureObject
{
private:
    AliasList aliases;
    std::string description;      

public:
    AdventureObject();
    virtual ~AdventureObject();
    AliasList& getAliases();
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player* player, bool startOfSentence = false) const;
    std::string getNameOnly(bool startOfSentence = false) const;
    bool isNamePlural() const;
    void setDescription(std::string description);
    std::string getDescription() const;
};

