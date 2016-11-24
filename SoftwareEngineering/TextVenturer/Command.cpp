#include "stdafx.h"

#include "Command.h"

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

Command::Result Command::testInput(string input)
{
    // search through all commands
    Result result;
    result.success = false;

    size_t inputPos = 0;
    size_t cmdPos = 0;
    for (string cmd : aliases)
    {
        // todo: replace " " to " +" and <.*> to (.+)

        strings params = extractParameters(input);
        regex r(cmd);   
        smatch matches;
        if (regex_search(input.cbegin(), input.cend(), matches, r))        
            for (int i = 0; i < matches.size(); i++)
                result.parameters[params[i]] = matches[i];                 
    }

    return result;
}

Command::Result::operator bool() const
{
    return success;
}

string Command::Result::operator[](string parameter)
{
    return parameters[parameter];
}
