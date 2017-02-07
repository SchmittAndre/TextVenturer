#include "stdafx.h"

#include "Controler.h"
#include "TextDisplay.h"

#include "BaseAction.h"

BaseAction::BaseAction(Controler * controler)
{
    this->controler = controler;
}

Controler * BaseAction::getControler() const
{
    return controler;
}

void BaseAction::write(const std::string & text) const
{
    controler->write(text);
}
