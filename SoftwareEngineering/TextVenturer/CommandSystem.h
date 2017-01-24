#pragma once

class Command;
class BaseAction;
class Controler;

struct CommandAction
{
    Command* command;
    BaseAction* action;

    CommandAction(Command* cmd, BaseAction* action);
};

class CommandSystem
{
private:
    struct ParamAction {
        BaseAction* action;
        Command::Result params;

        ParamAction(BaseAction* action, Command::Result params = Command::Result());
    };

    BaseAction* defaultAction;
    vector<CommandAction> commands;
    queue<string> commandQueue; 

public:
    CommandSystem(Controler* controler, BaseAction* defaultAction);

    void add(Command* cmd, BaseAction* a);
    void del(Command* cmd);

    void sendCommand(const string &input);

    void update();

    bool processingCommand();

};

