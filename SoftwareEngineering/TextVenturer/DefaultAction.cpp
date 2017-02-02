#include "stdafx.h"

#include "Controler.h"
#include "DefaultAction.h"

bool DefaultAction::run(const Command::Result & params) const
{
    getControler()->write("Don't confuse me!");
    return true;
}