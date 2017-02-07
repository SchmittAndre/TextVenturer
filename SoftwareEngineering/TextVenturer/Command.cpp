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
    std::smatch matches;
    for (auto pos = cmd.cbegin(); pos != cmd.cend(); pos++)
    {
        if (std::regex_search(pos, cmd.cend(), matches, std::regex("<(.+?)>"), std::regex_constants::match_continuous))
            result.push_back(matches[1]);
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

Command::Command(const std::string & cmd)
{
    parameters = extractParameters(cmd);
    addAlias(cmd);
}

Command::AddResult Command::addAlias(std::string alias)
{
    if (find(aliases.begin(), aliases.end(), alias) != aliases.end())
        return addExists;

    if (paramsToSet(extractParameters(alias)) != paramsToSet(parameters))
        return addIncompatible;

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
        cmd = std::regex_replace(cmd, std::regex("<prep>"), *prepositions);

        cmd = std::regex_replace(cmd, std::regex("<.*?>"), "(.+?)"); // <IDENTIFIER> to regex match syntax
        cmd = std::regex_replace(cmd, std::regex(" +"), " +");       // take any amount of spaces
        cmd = " *" + cmd + " *";                                     // can have any amount of spaces on either side

        std::smatch matches;
        if (std::regex_match(input, matches, std::regex(cmd, std::regex_constants::icase)))
        {
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
                result.parameters[parameters[i - 1]] = std::regex_replace((std::string)matches[i], std::regex(" +"), " ");
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