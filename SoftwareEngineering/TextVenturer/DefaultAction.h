#pragma once

#include "BaseAction.h"

class Command;

class DefaultAction : public BaseAction
{
public:
    DefaultAction(Controler* controler) : BaseAction(controler) {}
    void run(const Command::Result & params) const;
};