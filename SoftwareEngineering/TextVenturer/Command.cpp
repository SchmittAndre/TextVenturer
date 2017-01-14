#include "stdafx.h"

#include "Command.h"

// Command::Result

Command::Result::operator bool() const
{
    return success;
}

string Command::Result::operator[](const string & parameter) const
{
    return parameters.at(parameter);
}

bool Command::Result::hasParam(const string & parameter) const
{
    for (dictionary::const_iterator current = parameters.begin(); current != parameters.end(); current++)
        if (current->first == parameter)
            return true;
    return false;
}

// Command

strings Command::extractParameters(string cmd)
{                           
    // search for <IDENTIFIER> enclosed
    strings result;
    bool parsingIdent = false;
    string ident;
    for (char c : cmd)
    {
        if (parsingIdent)
        {
            if (c == '>')
            {
                result.push_back(ident);
                ident = "";
                parsingIdent = false;
            }
            else
                ident += c;
        }
        else if (c == '<')
            parsingIdent = true;
    }
    return result;
}

Command::Command(const string & cmd)
{
    for (string s : extractParameters(cmd))
        parameters.insert(s);             
    aliases.push_back(cmd);
}

Command::AddResult Command::addAlias(const string & alias)
{
    if (find(aliases.begin(), aliases.end(), alias) != aliases.end())
        return addExists;

    tags aliasParams;
    for (string s : extractParameters(alias))
        aliasParams.insert(s);
    if (aliasParams != parameters)
        return addIncompatible;

    aliases.push_back(alias);
    return addSuccess;
}

bool Command::delAlias(const string & alias)
{
    for (size_t i = 1; i < aliases.size(); i++)
        if (aliases[i] == alias)
        {
            aliases.erase(aliases.begin() + i);
            return true;
        }
    return false;
}

string Command::getName() const
{    
    return aliases[0];
}

strings Command::getAliases() const
{
    return aliases;
}

Command::Result Command::check(const string & input) const
{
    // search through all commands
    Result result;

    size_t inputPos = 0;
    size_t cmdPos = 0;
    for (string cmd : aliases)
    {
        strings params = extractParameters(cmd);
        
        cmd = regex_replace(cmd, regex("<.*?>"), "(.+?)"); // <IDENTIFIER> to regex match syntax
        cmd = regex_replace(cmd, regex(" +"), " +");       // take any amount of spaces
        cmd = " *" + cmd + " *";                           // can have any amount of spaces on either side

        smatch matches;
        if (regex_match(input, matches, regex(cmd, regex_constants::icase)))
        {
            result.parameters.clear();
            bool success = true;
            // first is whole string, because it matched so skip that with i = 1
            for (size_t i = 1; i < matches.size(); i++)
            {
                // we don't want any empty strings as parameters
                if (matches[i] == " ")
                {
                    success = false;
                    break;
                }
                result.parameters[params[i - 1]] = matches[i];
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