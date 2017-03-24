#pragma once
                    
#include "Command.h"

class CmdLine;

class BaseAction abstract
{
private:
    CmdLine* cmdLine;

public:
    BaseAction(CmdLine* cmdLine);
    virtual ~BaseAction();
    CmdLine* getCmdLine() const;
    virtual bool run(const Command::Result & params = Command::Result()) = 0;
    void write(const std::string & text) const;
    virtual tags requiredParameters() const { return{}; };
};