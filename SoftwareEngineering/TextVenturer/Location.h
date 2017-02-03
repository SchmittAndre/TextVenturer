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
    AliasList* aliases;
    std::string description;

    Inventory* inventory;

    std::vector<LocatedCommandAction> commands;

public:
    Location(std::string name, std::string description);
    virtual ~Location();

    AliasList* getAliases() const;
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player * player, bool startOfSentence = false) const;
    virtual std::string getDescription() const;

    void changeDescription(std::string description);   

    void addCommand(Command* command, CustomAdventureAction* action, bool anywhere);

    Inventory* getInventory();
};

