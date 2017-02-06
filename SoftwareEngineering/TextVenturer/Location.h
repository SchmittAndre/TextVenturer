#pragma once

#include "CommandSystem.h"
#include "CustomAdventureAction.h"
#include "Inventory.h"

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
    private:
        strings prepAliasesList;    // strings for list/put          
        strings prepAliasesTake;    // strings also useable for take
    public:
        bool addPrepositionAlias(std::string alias, bool take = false);
        bool delPrepositionAlias(std::string alias);
        std::string getPrepositionName(bool take = false, bool startOfSentence = false) const;
        bool hasPrepositionAlias(std::string alias, bool take = false) const;
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

    std::string formatPrepositions();
};

