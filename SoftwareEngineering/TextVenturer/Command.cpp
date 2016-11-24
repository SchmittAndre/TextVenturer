#include "stdafx.h"

#include "Command.h"

// Command::Result

Command::Result::operator bool() const
{
    return success;
}

string Command::Result::operator[](string parameter)
{
    return parameters[parameter];
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

Command::Command(string cmd)
{
    for (string s : extractParameters(cmd))
        parameters.insert(s);             
    aliases.push_back(cmd);
}

Command::AddResult Command::addAlias(string alias)
{
    for (int i = 0; i < aliases.size(); i++)
        if (aliases[i] == alias)
            return addExists;

    tags aliasParams;
    for (string s : extractParameters(alias))
        aliasParams.insert(s);
    if (aliasParams != parameters)
        return addIncompatible;

    aliases.push_back(alias);
    return addSuccess;
}

bool Command::delAlias(string alias)
{
    for (int i = 1; i < aliases.size(); i++)
        if (aliases[i] == alias)
        {
            aliases.erase(aliases.begin() + i);
            return true;
        }
    return false;
}

string Command::getName()
{    
    return aliases[0];
}

strings Command::getAliases()
{
    return aliases;
}

Command::Result Command::check(string input)
{
    // search through all commands
    Result result;

    size_t inputPos = 0;
    size_t cmdPos = 0;
    for (string cmd : aliases)
    {
        strings params = extractParameters(cmd);
        
        cmd = regex_replace(cmd, regex("<.*?>"), "(.+?)"); // <ODENTIFIER> to regex match syntax
        cmd = regex_replace(cmd, regex(" +"), " +");       // take any amount of spaces
        cmd = " *" + cmd + " *";                           // can have any amount of spaces on either side

        smatch matches;
        if (regex_match(input, matches, regex(cmd)))
        {
            result.parameters.clear();
            bool worked = true;
            // first is whole string, because it matched so skip that with i = 1
            for (int i = 1; i < matches.size(); i++)
            {
                // we don't want any empty strings as parameters
                if (matches[i] == " ")
                {
                    worked = false;
                    break;
                }
                result.parameters[params[i - 1]] = matches[i];
            }
            if (worked)
            {
                result.success = true;
                return result;
            }
        }
    }

    result.success = false;
    return result;
}