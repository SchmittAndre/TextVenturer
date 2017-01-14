#include "stdafx.h"
#include "Command.h"
#include "BaseAction.h"

#include "CommandSystem.h"

CommandAction::CommandAction(Command* cmd, BaseAction* action)
{
    this->cmd = cmd;
    this->action = action;
}

CommandSystem::CommandSystem(Controler* controler, BaseAction * defaultAction)
{
    this->defaultAction = defaultAction;
}

void CommandSystem::add(Command* cmd, BaseAction* action)
{
    commands.push_back(CommandAction(cmd, action));
}

void CommandSystem::del(Command * cmd)
{
    vector<CommandAction>::iterator current;
    for (current = commands.begin(); current != commands.end(); current++)
        if (current->cmd == cmd)
            break;  
    if (current != commands.end())
        commands.erase(current);
}

void CommandSystem::sendCommand(const string & input) const
{
    for (CommandAction current : commands)
        if (Command::Result result = current.cmd->check(input))
        {
            current.action->run(result);
            return;
        }

    defaultAction->run();
}
