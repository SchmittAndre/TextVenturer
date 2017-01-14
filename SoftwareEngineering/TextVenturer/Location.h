#pragma once

class Item;
class AdventureAction;

class Location
{
private:
    AliasList* aliases;
    string description;

    unordered_map<Item*, AdventureAction*> useActions;

public:
    Location(string name, string description);
    virtual ~Location();

    AliasList* getAliases() const;
    string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    virtual string getDescription() const;

    void changeDescription(string description);

    bool addItemAction(Item* item, AdventureAction* action);
    bool delItemAction(Item* item);
    AdventureAction* getItemAction(Item* item) const;
};

