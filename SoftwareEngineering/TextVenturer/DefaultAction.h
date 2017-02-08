#pragma once

#include "BaseAction.h"
#include "Command.h"

class Controler;

class DefaultAction : public BaseAction
{
public:
    DefaultAction(Controler* controler) : BaseAction(controler) {}
    bool run(const Command::Result & params) const;
};