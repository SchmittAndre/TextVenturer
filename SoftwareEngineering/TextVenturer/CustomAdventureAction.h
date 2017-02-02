#pragma once

#include "AdventureAction.h"

class Adventure;
class AdventureAction;
class Command;

class CustomAdventureAction : public AdventureAction
{
private:
    string script;

public:
    CustomAdventureAction(Adventure* adventure, string script);
    bool run(const Command::Result & params = Command::Result()) const;
};

