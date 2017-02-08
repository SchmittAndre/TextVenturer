#pragma once

#include "Inventory.h"
#include "CommandSystem.h"

class Command;
class CustomAdventureAction;
class Item;
class Player;

class Location
{
public:
    struct LocatedCommandAction
    {
        bool anywhere;                  
        CommandAction commandAction;
        LocatedCommandAction(Command* command, CustomAdventureAction* action, bool anywhere);
    };

    class PInventory : public Inventory
    {
    public:
        enum Filter
        {
            ifBlacklist,
            ifWhitelist
        };
    private:
        strings prepAliasesList;    // strings for list/put          
        strings prepAliasesTake;    // strings also useable for take
        Inventory* filter;
        Filter mode;
    public:
        PInventory();
        ~PInventory();

        bool addPrepositionAlias(std::string alias, bool take = false);
        bool delPrepositionAlias(std::string alias);
        std::string getPrepositionName(bool take = false, bool startOfSentence = false) const;
        bool hasPrepositionAlias(std::string alias, bool take = false) const;

        bool addItem(Item* item);

        bool isFiltered() const;
        Filter getFilterMode() const;
        void enableFilter(Filter mode);
        void addToFilter(Item* item);
        bool delFromFilter(Item* item);
    };

private:
    AliasList aliases;
    std::string description;

    std::vector<LocatedCommandAction> commands;
    std::unordered_map<std::string, PInventory*> inventories;

public:
    virtual ~Location();

    AliasList& getAliases();
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player * player, bool startOfSentence = false) const;
    void setDescription(std::string description);   
    std::string getDescription() const;         

    void addCommand(Command* command, CustomAdventureAction* action, bool anywhere);         

    PInventory* addInventory(std::string preposition);
    bool delInventory(std::string preposition);
    bool hasInventory(std::string preposition);
    size_t filledInventoryCount();
    PInventory* firstFilledInventory();
    std::vector<PInventory*> getInventories();
    
    PInventory* getInventory(std::string preposition);

    std::string formatPrepositions(bool filledOnly = false);
};

