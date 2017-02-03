#pragma once

#include "Command.h"
#include "BaseAction.h"

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
    std::vector<CommandAction> commands;
    std::queue<std::string> commandQueue; 

public:
    CommandSystem(Controler* controler, BaseAction* defaultAction);

    void add(Command* cmd, BaseAction* a);
    void del(Command* cmd);

    void sendCommand(const std::string &input);

    void update();

    bool processingCommand();

};

