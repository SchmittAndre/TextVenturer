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
    string description;

    Inventory* inventory;

    vector<LocatedCommandAction> commands;

public:
    Location(string name, string description);
    virtual ~Location();

    AliasList* getAliases() const;
    string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    string getName(Player * player, bool startOfSentence = false) const;
    virtual string getDescription() const;

    void changeDescription(string description);   

    void addCommand(Command* command, CustomAdventureAction* action, bool anywhere);

    Inventory* getInventory();
};

