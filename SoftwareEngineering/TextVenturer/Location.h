#pragma once

#include "CommandSystem.h"
#include "CustomAdventureAction.h"

class Location
{
public:
    struct LocatedCommandAction
    {
        bool anywhere;                  
        CommandAction commandAction;
        LocatedCommandAction(Command* command, CustomAdventureAction* action, bool anywhere);
    };

private:
    AliasList aliases;
    std::string description;

    Inventory* inventory;

    std::vector<LocatedCommandAction> commands;

public:
    Location();
    virtual ~Location();

    AliasList& getAliases();
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player * player, bool startOfSentence = false) const;
    void setDescription(std::string description);   
    std::string getDescription() const;


    void addCommand(Command* command, CustomAdventureAction* action, bool anywhere);

    Inventory* getInventory();
};

