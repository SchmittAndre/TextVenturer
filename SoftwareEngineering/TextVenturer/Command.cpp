#include "stdafx.h"

#include "Command.h"

// Command::Result

Command::Result::operator bool() const
{
    return success;
}  

std::string Command::Result::operator[](const std::string & parameter) const
{
    return parameters.at(parameter);
}

bool Command::Result::hasParam(const std::string & parameter) const
{
    for (dictionary::const_iterator current = parameters.begin(); current != parameters.end(); current++)
        if (current->first == parameter)
            return true;
    return false;
}

// Command     

strings Command::extractParameters(std::string cmd)
{                           
    // search for <IDENTIFIER> enclosed
    strings result;
    std::string ident;
    size_t pos = 0, right;
    while (true)
    {
        pos = cmd.find('<', pos);
        if (pos == std::string::npos)
            break;
        pos++;
        right = cmd.find('>', pos);
        if (right == std::string::npos)
            break;
        result.push_back(cmd.substr(pos, right - pos));
        pos++;
    }
    return result;
}

tags Command::paramsToSet(strings params)
{
    tags result;
    for (std::string param : params)
        result.insert(param);
    return result;
}

void Command::save(FileStream & stream) const
{
    stream.write(aliases);
}

Command::Command(FileStream & stream)
{
    stream.read(aliases);
}

Command::Command()
{
}

Command::AddResult Command::addAlias(std::string alias)
{
    if (aliases.empty())
    {
        parameters = paramsToSet(extractParameters(alias));
    }
    else
    {
        if (find(aliases.begin(), aliases.end(), alias) != aliases.end())
            return addExists;

        if (paramsToSet(extractParameters(alias)) != parameters)
            return addIncompatible;
    }

    aliases.push_back(alias);
    return addSuccess;
}

bool Command::delAlias(const std::string & alias)
{
    for (size_t i = 1; i < aliases.size(); i++)
        if (aliases[i] == alias)
        {
            aliases.erase(aliases.begin() + i);
            return true;
        }
    return false;
}

void Command::setPrepositions(std::string* prepositions)
{
    this->prepositions = prepositions;
}

std::string Command::getName() const
{    
    return aliases[0];
}      

strings Command::getAliases() const
{
    return aliases;
}

Command::Result Command::check(const std::string & input) const
{
    // search through all commands
    Result result;

    size_t inputPos = 0;
    size_t cmdPos = 0;
    for (std::string cmd : aliases)
    {                      
        std::string changedCmd = std::regex_replace(cmd, std::regex("<prep>"), *prepositions);

        changedCmd = std::regex_replace(changedCmd, std::regex("<.*?>"), "(.+?)"); // <IDENTIFIER> to regex match syntax
        changedCmd = std::regex_replace(changedCmd, std::regex(" +"), " +");       // take any amount of spaces
        changedCmd = " *" + changedCmd + " *";                                     // can have any amount of spaces on either side

        std::smatch matches;
        if (std::regex_match(input, matches, std::regex(changedCmd, std::regex_constants::icase)))
        {
            strings params = extractParameters(cmd);
            
            result.parameters.clear();
            bool success = true;
            // first is whole std::string, because it matched so skip that with i = 1
            for (size_t i = 1; i < matches.size(); i++)
            {
                // we don't want any empty strings as parameters
                if (matches[i] == " ")
                {
                    success = false;
                    break;
                }
                result.parameters[params[i - 1]] = std::regex_replace((std::string)matches[i], std::regex(" +"), " ");
            }
            if (success)
            {
                result.success = true;
                return result;
            }
        }
    }

    result.success = false;
    return result;
}