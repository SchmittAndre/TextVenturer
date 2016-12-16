#pragma once
                      
class Controler;
class Command;
class CommandSystem;

class BaseAction abstract
{
    friend CommandSystem;
private:
    Controler* controler;
public:
    Controler* getControler();
    virtual void run(Command::Result params = Command::Result()) = 0;
};