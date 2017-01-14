#include "stdafx.h"
#include "Controler.h"
#include "Command.h"
#include "Adventure.h"

#include "AdventureAction.h"

AdventureAction::AdventureAction(Adventure * adventure)
:   BaseAction(adventure->getControler())
{
    this->adventure = adventure;
}

Adventure * AdventureAction::getAdventure() const
{
    return adventure;
}
