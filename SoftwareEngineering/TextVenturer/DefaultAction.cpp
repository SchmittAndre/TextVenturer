#include "stdafx.h"

#include "CmdLine.h"

#include "DefaultAction.h"

bool DefaultAction::run(const Command::Result & params) 
{
    getCmdLine()->write("Don't confuse me!");
    return true;
}