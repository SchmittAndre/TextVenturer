#include "stdafx.h"
#include "Adventure.h"
#include "Command.h"

#include "CustomAdventureAction.h"

CustomAdventureAction::CustomAdventureAction(Adventure * adventure, string script)
:   AdventureAction(adventure)
{
    this->script = script;
}

void CustomAdventureAction::run(const Command::Result & params) const
{



}
