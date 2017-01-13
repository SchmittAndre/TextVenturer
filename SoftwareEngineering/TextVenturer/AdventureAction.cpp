#include "stdafx.h"
#include "Adventure.h"

#include "AdventureAction.h"

AdventureAction::AdventureAction(Adventure * adventure)
{
    this->adventure = adventure;
}

Adventure * AdventureAction::getAdventure()
{
    return adventure;
}
