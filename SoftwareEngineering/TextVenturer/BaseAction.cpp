#include "stdafx.h"

#include "CmdLine.h"
#include "TextDisplay.h"

#include "BaseAction.h"

BaseAction::BaseAction(CmdLine * cmdLine)
{
    this->cmdLine = cmdLine;
}

BaseAction::~BaseAction()
{
}

CmdLine * BaseAction::getCmdLine() const
{
    return cmdLine;
}

void BaseAction::write(const std::string & text) const
{
    cmdLine->write(text);
}
