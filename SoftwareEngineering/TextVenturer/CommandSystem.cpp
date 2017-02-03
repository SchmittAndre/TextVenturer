#include "stdafx.h"

#include "CommandSystem.h"

CommandAction::CommandAction(Command* cmd, BaseAction* action)
{
    this->command = cmd;
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
    std::vector<CommandAction>::iterator current;
    for (current = commands.begin(); current != commands.end(); current++)
        if (current->command == cmd)
            break;  
    if (current != commands.end())
        commands.erase(current);
}

void CommandSystem::sendCommand(const std::string & input) 
{
    commandQueue.push(input);  
}

void CommandSystem::update()
{
    if (commandQueue.size() > 0 && !processingCommand())
    {
        // only one command/thread at a time, otherwise the order might get mixed up
        std::string input = commandQueue.front();
        commandQueue.pop();
        
        std::thread([this, input]()
        {
            for (CommandAction current : commands)
            {
                if (Command::Result params = current.command->check(input))
                    if (current.action->run(params))
                        return;                                                                
            }
            defaultAction->run();
        }).detach();
    } 
}

bool CommandSystem::processingCommand()
{
    return commandQueue.size() > -1;
}

