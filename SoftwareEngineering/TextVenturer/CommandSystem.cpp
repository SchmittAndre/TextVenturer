#include "stdafx.h"

#include "Command.h"
#include "BaseAction.h"
#include "Controler.h"

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

void CommandSystem::genPrepositions()
{
    if (prepositions.size() == 0)
        prepositionRegexString = "(.+?)";
    else
    {
        prepositionRegexString = "(";
        for (auto entry : prepositions)
            prepositionRegexString += entry + "|";
        prepositionRegexString.pop_back();
        prepositionRegexString += ")";
    }
}

CommandSystem::CommandSystem(Controler* controler, BaseAction * defaultAction)
{
    this->defaultAction = defaultAction;
    genPrepositions();
}

void CommandSystem::add(Command* cmd, BaseAction* action)
{
    commands.add(cmd, action);
    cmd->setPrepositions(&prepositionRegexString);
}

void CommandSystem::add(CommandArray* commandArray)
{
    commandArrays.push_back(commandArray);
    for (CommandAction cmd : *commandArray)
        cmd.command->setPrepositions(&prepositionRegexString);
}

void CommandSystem::del(Command * cmd)
{
    commands.del(cmd);
}

void CommandSystem::del(CommandArray* commandSet)
{
    auto pos = std::find(commandArrays.begin(), commandArrays.end(), commandSet);
    if (pos != commandArrays.end())
        commandArrays.erase(pos);
}

void CommandSystem::addPreposition(std::string preposition)
{
    prepositions.insert(preposition);
    genPrepositions();
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
            for (CommandArray* commandArray : commandArrays)
                if (commandArray->sendCommand(input))
                    return;

            if (commands.sendCommand(input))
                return;

            defaultAction->run();
        }).detach();
    } 
}

bool CommandSystem::processingCommand()
{
    return commandQueue.size() > -1;
}

CommandArray::CommandArray(bool referenced)
{
    this->referenced = referenced;
}

CommandArray::~CommandArray()
{
    if (!referenced)
    {
        for (CommandAction cmd : commands)
        {
            delete cmd.action;
            delete cmd.command;
        }
    }
}

void CommandArray::add(Command * cmd, BaseAction * action)
{
    if (Command::paramsToSet(Command::extractParameters(cmd->getName())) != action->requiredParameters())
    {
        ErrorDialog("Command \"" + cmd->getName() + "\"" + " is incompatible with its action.");
    }
    else
    {
        commands.push_back(CommandAction(cmd, action));
    }
}

void CommandArray::del(Command * cmd)
{
    std::vector<CommandAction>::iterator current;
    for (current = commands.begin(); current != commands.end(); current++)
        if (current->command == cmd)
            break;
    if (current != commands.end())
        commands.erase(current);
}

bool CommandArray::sendCommand(std::string input)
{
    for (CommandAction current : commands)
        if (Command::Result params = current.command->check(input))
            if (current.action->run(params))
                return true;
    return false;
}

std::vector<CommandAction>::iterator CommandArray::begin()
{
    return commands.begin();
}

std::vector<CommandAction>::iterator CommandArray::end()
{
    return commands.end();
}
