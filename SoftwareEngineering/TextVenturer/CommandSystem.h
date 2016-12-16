#pragma once

class Command;
class BaseAction;

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
    CommandSystem(BaseAction* defautlAction);

    void add(Command* cmd, BaseAction* a);
    void del(Command* cmd);

    void sendCommand(string input);

};

