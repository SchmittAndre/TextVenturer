#include "stdafx.h"
#include "Command.h"         
#include "TextDisplay.h"
#include "Controler.h"

#include "DefaultAction.h"

void DefaultAction::run(const Command::Result & params) const
{
    getControler()->write("Don't confuse me!");
}