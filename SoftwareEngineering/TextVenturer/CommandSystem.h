#pragma once

class Command;
class BaseAction;
class Controler;

struct CommandAction
{
    Command* cmd;
    BaseAction* action;

    CommandAction(Command* cmd, BaseAction* action);
};

class CommandSystem
{
private:
    BaseAction* defaultAction;
    vector<CommandAction> commands;

public:
    CommandSystem(Controler* controler, BaseAction* defaultAction);

    void add(Command* cmd, BaseAction* a);
    void del(Command* cmd);

    void sendCommand(const string &input) const;

};

