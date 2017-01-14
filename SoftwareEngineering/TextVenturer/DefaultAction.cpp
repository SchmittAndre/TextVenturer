#include "stdafx.h"
#include "Command.h"
#include "Controler.h"

#include "DefaultAction.h"

void DefaultAction::run(const Command::Result & params) const
{
    getControler()->textscrolling("Don't confuse me!");
}