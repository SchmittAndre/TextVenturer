#pragma once

#include "BaseAction.h"
#include "Command.h"

class CmdLine;

class DefaultAction : public BaseAction
{
public:
    DefaultAction(CmdLine* cmdLine) : BaseAction(cmdLine) {}
    bool run(const Command::Result & params);
};