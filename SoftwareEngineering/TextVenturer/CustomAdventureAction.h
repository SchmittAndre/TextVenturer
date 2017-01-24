#pragma once

class Adventure;
class Command;

#include "AdventureAction.h"

class CustomAdventureAction : public AdventureAction
{
private:
    string script;

public:
    CustomAdventureAction(Adventure* adventure, string script);
    void run(const Command::Result & params = Command::Result()) const;
};

