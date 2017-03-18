#include "stdafx.h"

#include "Adventure.h"
#include "CustomScript.h"

#include "CustomAdventureAction.h"

// CustomAdventureAction

CustomAdventureAction::CustomAdventureAction(Adventure * adventure, std::string code, std::string title, bool overrideDefault)
    : AdventureAction(adventure)
{
    script = new CustomScript::Script(this, code, title);
    this->overrideDefault = overrideDefault;
}

CustomAdventureAction::~CustomAdventureAction()
{
    delete script;
}

bool CustomAdventureAction::overrides() const
{
    return overrideDefault;
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

void CustomAdventureAction::save(FileStream & stream)
{
    stream.write(overrideDefault);
    script->save(stream);
}
