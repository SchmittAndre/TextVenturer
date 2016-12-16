#pragma once

#include "BaseAction.h"

class Command;

class DefaultAction : public BaseAction
{
public:
    void run(Command::Result params);
};