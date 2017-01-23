#include "stdafx.h"
#include "Command.h"
#include "BaseAction.h"

#include "CommandSystem.h"

CommandAction::CommandAction(Command* cmd, BaseAction* action)
{
    this->cmd = cmd;
    this->action = action;
}

CommandSystem::ParamAction::ParamAction(BaseAction * action, Command::Result params)
{
    this->action = action;
    this->params = params;
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

void CommandSystem::sendCommand(const string & input) 
{
    commandQueue.push(input);  
}

void CommandSystem::update()
{
    if (commandQueue.size() > 0 && !processingCommand())
    {
        // only one command/thread at a time, otherwise the order might get mixed up
        string input = commandQueue.front();
        commandQueue.pop();
        
        thread([this, input]()
        {
            for (CommandAction current : commands)
            {
                if (Command::Result params = current.cmd->check(input))
                {
                    findResults.push(ParamAction(current.action, params));
                    return;
                }
            }
            findResults.push(ParamAction(defaultAction));
        }).detach();
    } 
    while (findResults.size() > 0)
    {
        ParamAction tmp = findResults.front();
        tmp.action->run(tmp.params);
        findResults.pop();
    }
}

bool CommandSystem::processingCommand()
{
    return commandQueue.size() > -1;
}

