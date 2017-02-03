#include "stdafx.h"

#include "CustomAdventureAction.h"

CustomAdventureAction::CustomAdventureAction(Adventure * adventure, std::string script)
:   AdventureAction(adventure)
{
    this->script = script;
}

bool CustomAdventureAction::run(const Command::Result & params) const
{   
    return true;
}
