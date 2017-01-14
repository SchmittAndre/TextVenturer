#pragma once
                      
class Controler;
class Command;
class CommandSystem;

class BaseAction abstract
{
private:
    Controler* controler;

public:
    BaseAction(Controler* controler);
    Controler* getControler() const;
    virtual void run(const Command::Result & params = Command::Result()) const = 0;
    void write(const string & text) const;
};