#include "stdafx.h"

#include "Controler.h"

#include "DefaultAction.h"

bool DefaultAction::run(const Command::Result & params) 
{
    getControler()->write("Don't confuse me!");
    return true;
}