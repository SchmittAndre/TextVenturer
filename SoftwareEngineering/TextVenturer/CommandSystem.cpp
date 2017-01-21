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
    threadCount = 8;
    runningThreads = -1;
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
        string input = commandQueue.front();
        commandQueue.pop();
        
        size_t usedThreads = min(threadCount, commands.size());
        size_t batchSize = commands.size() / usedThreads;
        runningThreads = usedThreads;
        for (size_t i = 0; i < usedThreads; i++)
        {
            thread([this, input](size_t offset, size_t count)
            {
                for (size_t i = offset; i < offset + count && runningThreads != -1; i++)
                {
                    if (Command::Result params = commands[i].cmd->check(input))
                    {
                        findResults.push(ParamAction(commands[i].action, params));
                        runningThreads = -1;
                        return;
                    }
                }
                if (runningThreads != -1)
                    runningThreads--;
            }, i * batchSize, i == usedThreads - 1 ? batchSize + commands.size() % usedThreads : batchSize).detach();
        }
    } 
    if (runningThreads == 0) 
    {
        findResults.push(ParamAction(defaultAction));
        runningThreads = -1;
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
    return runningThreads != -1;
}

