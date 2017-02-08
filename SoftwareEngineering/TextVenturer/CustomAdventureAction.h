#pragma once

#include "AdventureAction.h"
#include "Command.h"

class Adventure;

class CustomAdventureAction : public AdventureAction
{
private:
    std::string script;

public:
    CustomAdventureAction(Adventure* adventure, std::string script);
    bool run(const Command::Result & params = Command::Result()) const;
};

