#pragma once

#include "AdventureAction.h"
#include "Command.h"
#include "CustomScript.h"

class Adventure;

class CustomAdventureAction : public AdventureAction
{
private:
    CustomScript::Script* script;

public:
    CustomAdventureAction(Adventure* adventure, std::string code, std::string title);
    ~CustomAdventureAction();
   
    tags requiredParameters() const;
    bool compileSucceeded() const;
    bool run(const Command::Result & params = Command::Result());
};