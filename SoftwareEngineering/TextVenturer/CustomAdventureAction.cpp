#include "stdafx.h"

#include "Adventure.h"
#include "CustomScript.h"

#include "CustomAdventureAction.h"

// CustomAdventureAction

CustomAdventureAction::CustomAdventureAction(Adventure * adventure, std::string code, std::string title)
    : AdventureAction(adventure)
{
    script = new CustomScript::Script(this, code, title);
}

CustomAdventureAction::~CustomAdventureAction()
{
    delete script;
}

tags CustomAdventureAction::requiredParameters() const
{
    return script->getRequiredParams();
}

bool CustomAdventureAction::compileSucceeded() const
{
    return script->succeeded();
}

bool CustomAdventureAction::run(const Command::Result & params)
{
    return script->run(params);
}
