#pragma once

#include "BaseAction.h"

class BaseAction;
class Command;

class DefaultAction : public BaseAction
{
public:
    DefaultAction(Controler* controler) : BaseAction(controler) {}
    bool run(const Command::Result & params) const;
};