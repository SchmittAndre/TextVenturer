#include "stdafx.h"

#include "Adventure.h"
#include "CustomScript.h"

#include "CustomAdventureAction.h"

// CustomAdventureAction

CustomAdventureAction::CustomAdventureAction(FileStream & stream, Adventure & adventure)
    : AdventureAction(adventure)
    , script(*this, stream)
    , overrideDefault(stream.readBool())
{
}

CustomAdventureAction::CustomAdventureAction(Adventure & adventure, std::string code, std::string title, bool overrideDefault)
    : AdventureAction(adventure)
    , script(*this, code, title)
{
    this->overrideDefault = overrideDefault;
}

bool CustomAdventureAction::overrides() const
{
    return overrideDefault;
}

taglist CustomAdventureAction::requiredParameters() const
{
    return script.getRequiredParams();
}

bool CustomAdventureAction::compileSucceeded() const
{
    return script.succeeded();
}

bool CustomAdventureAction::run(const Command::Result & params)
{
    return script.run(params);
}

void CustomAdventureAction::save(FileStream & stream) const
{
    script.save(stream);
    stream.write(overrideDefault);
}
