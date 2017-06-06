#include "stdafx.h"

#include "Command.h"
#include "AdventureAction.h"
#include "Controler.h"
#include "CustomAdventureAction.h"
#include "Adventure.h"
#include "CmdLine.h"
#include "Game.h"
#include "Window.h"

#include "CommandSystem.h"

CommandAction::CommandAction(Command & command, AdventureAction & action)
    : command(&command)
    , action(&action)
{
}

CommandSystem::ParamAction::ParamAction(AdventureAction & action, Command::Result params)
    : action(action)
    , params(params)
{
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

CommandSystem::CommandSystem(AdventureAction & defaultAction)
    : defaultAction(defaultAction)
    , commands(true)
{
    genPrepositions();
}

void CommandSystem::add(Command & cmd, AdventureAction & action)
{
    commands.add(cmd, action);
    cmd.setPrepositions(prepositionRegexString);
}

void CommandSystem::add(CommandArray & commandArray)
{
    commandArrays.push_back(commandArray);
    for (const CommandAction & cmd : commandArray)
        cmd.command->setPrepositions(prepositionRegexString);
}

void CommandSystem::del(Command & cmd)
{
    commands.del(cmd);
}

void CommandSystem::del(CommandArray & commandArray)
{
    auto pos = std::find_if(commandArrays.begin(), commandArrays.end(), [&](CommandArray & value) 
    { 
        return &commandArray == &value;
    });
    if (pos != commandArrays.end())
        commandArrays.erase(pos);
    else
        throw(ECommandArrayDoesNotExist);
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
            try
            {
                for (CommandArray & commandArray : commandArrays)
                    if (commandArray.sendCommand(input))
                        return;

                if (commands.sendCommand(input))
                    return;

                defaultAction.run();
            }
            catch (...)
            {
                defaultAction.getCmdLine().getControler().getGame().getWindow().showException();
            }
        }).detach();
    } 
}

bool CommandSystem::processingCommand() const
{
    return commandQueue.size() > 0;
}

void CommandSystem::save(FileStream & stream, AdventureSaveHelp & help) const
{
    stream.write(static_cast<UINT>(commandArrays.size()));
    for (CommandArray & commandArray : commandArrays)
        stream.write(help.commandArrays[&commandArray]);

    stream.write(static_cast<UINT>(prepositions.size()));
    for (std::string preposition : prepositions)
        stream.write(preposition);
}

void CommandSystem::load(FileStream & stream, AdventureLoadHelp & help)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        commandArrays.push_back(help.commandArrays[stream.readUInt()]);

    length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        prepositions.insert(stream.readString());
    genPrepositions();
}

CommandArray::CommandArray(bool referenced)
    : referenced(referenced)
{
}

CommandArray::CommandArray(FileStream & stream, AdventureLoadHelp & help, bool referenced)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
    {
        Command command(stream);
        CustomAdventureAction action(stream, help.adventure);
        commands.push_back(CommandAction(command, action));
    }
}

CommandArray::~CommandArray()
{
    if (!referenced)
        for (auto & command : commands)
        {
            delete command.command;
            delete command.action;
        }
}

void CommandArray::add(Command & cmd, AdventureAction & action)
{
    // test if all action required params are in the command
    // by removing all params in command from the action
    taglist commandParams = action.requiredParameters();
    for (std::string param : Command::paramsToSet(Command::extractParameters(cmd.getName())))
        commandParams.erase(param);

    if (commandParams.empty())
        commands.push_back(CommandAction(cmd, action));
    else
    {
        std::string params;
        for (std::string p : commandParams)
            params += "\n  <" + p + ">";
        throw(ECommandMissingParameters, cmd, params);
        // ErrorDialog();
    }
}

void CommandArray::del(Command & cmd)
{
    auto pos = std::find_if(commands.begin(), commands.end(), [&](CommandAction & a)
    {
        return &cmd == a.command;
    });
    if (pos != commands.cend())
        commands.erase(pos);
    else
        throw(ECommandDoesNotExist, cmd);
}

bool CommandArray::sendCommand(std::string input)
{
    for (CommandAction & current : commands)
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

void CommandArray::save(FileStream & stream, AdventureSaveHelp & help) const
{
    stream.write(static_cast<UINT>(commands.size()));
    for (auto & command : commands)
    {
        command.command->save(stream);
        dynamic_cast<CustomAdventureAction&>(*command.action).save(stream);
    }
}

ECommandDoesNotExist::ECommandDoesNotExist(const Command & cmd)
    : Exception("Command \"" + cmd.getName() + "\" does not exist in command array")
{
}

ECommandMissingParameters::ECommandMissingParameters(const Command & cmd, const std::string & params)
    : Exception("Command \"" + cmd.getName() + "\" is missing parameters:" + params)
{
}

ECommandArrayDoesNotExist::ECommandArrayDoesNotExist()
    : Exception("Command array does not exist")
{
}
