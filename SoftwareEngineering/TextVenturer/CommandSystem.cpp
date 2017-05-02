#include "stdafx.h"

#include "Command.h"
#include "AdventureAction.h"
#include "Controler.h"
#include "CustomAdventureAction.h"
#include "Adventure.h"

#include "CommandSystem.h"

CommandAction::CommandAction(Command* cmd, AdventureAction* action)
{
    this->command = cmd;
    this->action = action;
}

CommandSystem::ParamAction::ParamAction(AdventureAction * action, Command::Result params)
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

CommandSystem::CommandSystem()
{
    defaultAction = NULL;
    genPrepositions();
}

CommandSystem::~CommandSystem()
{
    delete defaultAction;
}

void CommandSystem::setDefaultAction(AdventureAction * action)
{
    defaultAction = action;
}

bool CommandSystem::add(Command* cmd, AdventureAction* action)
{
    if (commands.add(cmd, action))
    {
        cmd->setPrepositions(&prepositionRegexString);
        return true;
    }
    return false;
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
    if (processingCommand())
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
    return commandQueue.size() > 0;
}

void CommandSystem::save(FileStream & stream, idlist<CommandArray*> & commandArrayIDs)
{
    stream.write(static_cast<UINT>(commandArrays.size()));
    for (CommandArray* commandArray : commandArrays)
        stream.write(commandArrayIDs[commandArray]);

    stream.write(static_cast<UINT>(prepositions.size()));
    for (std::string preposition : prepositions)
        stream.write(preposition);
}

void CommandSystem::load(FileStream & stream, std::vector<CommandArray*>& commandArrayList)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        commandArrays.push_back(commandArrayList[stream.readUInt()]);

    length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        prepositions.insert(stream.readString());
    genPrepositions();
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

bool CommandArray::add(Command * cmd, AdventureAction * action)
{
    // test if all action required params are in the command
    // by removing all params in command from the action
    tags commandParams = action->requiredParameters();
    for (std::string param : Command::paramsToSet(Command::extractParameters(cmd->getName())))
        commandParams.erase(param);
    if (!commandParams.empty())
    {
        std::string params;
        for (std::string p : commandParams)
            params += "\n  <" + p + ">";
        ErrorDialog("Command \"" + cmd->getName() + "\" is missing parameters:" + params);
        return false;
    }
    else
    {
        commands.push_back(CommandAction(cmd, action));
        return true;
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

void CommandArray::save(FileStream & stream)
{
    stream.write(static_cast<UINT>(commands.size()));
    for (auto command : commands)
    {
        command.command->save(stream);
        static_cast<CustomAdventureAction*>(command.action)->save(stream);
    }
}

void CommandArray::load(FileStream & stream, Adventure * adventure)
{
    UINT length = stream.readUInt();    
    for (UINT i = 0; i < length; i++)
    {
        Command* command = new Command(stream);
        CustomAdventureAction* action = new CustomAdventureAction(stream, adventure);
        commands.push_back(CommandAction(command, action));
    }
}
