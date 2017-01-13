#include "stdafx.h"
#include "Controler.h"
#include "Command.h"
#include "CommandSystem.h"

#include "BaseAction.h"

Controler * BaseAction::getControler()
{
    return controler;
}

void BaseAction::write(string text)
{
    controler->textscrolling(text);
}
