#pragma once

class BaseAction abstract
{
public:
    virtual void run(Command::Result params = Command::Result()) = 0;
};