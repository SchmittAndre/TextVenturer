#include "stdafx.h"
#include "Command.h"
#include "Controler.h"

#include "DefaultAction.h"

void DefaultAction::run(Command::Result params)
{
    getControler()->textscrolling("Wait... What?");
}