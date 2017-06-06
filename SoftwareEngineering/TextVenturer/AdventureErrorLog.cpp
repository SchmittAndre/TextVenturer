#include "stdafx.h"

#include "Adventure.h"

#include "AdventureErrorLog.h"

AdventureErrorLog::AdventureErrorLog(Controler & controler)
    : GameDisplayer(controler)
{
}

void AdventureErrorLog::setAdventure(Adventure & adventure)
{
    this->adventure = &adventure;
}

void AdventureErrorLog::notifyLoad()
{
    assert(adventure);


}

void AdventureErrorLog::notifyUnload()
{
    adventure = NULL;
}
