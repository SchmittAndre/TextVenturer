#include "stdafx.h"
#include "Command.h"
#include "Controler.h"

#include "DefaultAction.h"

void DefaultAction::run(const Command::Result & params) const
{
    getControler()->writeLine("Don't confuse me!");
}