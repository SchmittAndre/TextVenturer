#include "stdafx.h"
#include "Controler.h"
#include "Command.h"
#include "CommandSystem.h"

#include "BaseAction.h"

BaseAction::BaseAction(Controler * controler)
{
    this->controler = controler;
}

Controler * BaseAction::getControler() const
{
    return controler;
}

void BaseAction::write(const string & text) const
{
    controler->writeLine(text);
}
