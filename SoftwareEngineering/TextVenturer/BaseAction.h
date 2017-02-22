#pragma once
                    
#include "Command.h"

class Controler;

class BaseAction abstract
{
private:
    Controler* controler;

public:
    BaseAction(Controler* controler);
    virtual ~BaseAction();
    Controler* getControler() const;
    virtual bool run(const Command::Result & params = Command::Result()) = 0;
    void write(const std::string & text) const;
    virtual tags requiredParameters() const { return{}; };
};