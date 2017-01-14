#include "stdafx.h"

#include "AliasList.h"

Alias::Alias(string name, bool isPlural)
{
    if (name.substr(0, 4) == "the ")
        name = name.substr(4);
    else if (name.substr(0, 2) == "a ")
        name = name.substr(2);
    else if (name.substr(0, 3) == "an ")
        name = name.substr(3);
    name = name.substr(name.find_first_not_of(' '));
    this->name = name;
    this->plural = isPlural;
}

bool Alias::startsWithVowel() const
{
    if (name == "")
        return false;
    return name[0] == 'a' ||
           name[0] == 'e' ||
           name[0] == 'i' ||
           name[0] == 'o' ||
           name[0] == 'u';
}

bool Alias::isCompatible(string name) const
{
    // This works obviously, but you can't write "take stone" and need the article
    // return name == generate(true) || 
    //        name == generate(false);

    // Better is to just allow everything for checking.
    // Some might fail to differntiate "a" and "an" even though "take a stone" sounds stupid anyway...
    // But it will give you an error message, that there is no stone if we forbid this...
    return name == "the " + nameOnly() ||
           name == "a " + nameOnly() ||
           name == "an " + nameOnly() ||
           name == nameOnly();
}

string Alias::getArticle(bool definiteArticle) const
{
    return definiteArticle ? "the " : (plural ? "" : "a ");
}


string Alias::generate(bool definiteArticle, bool startOfSentence) const
{
    string result = getArticle(definiteArticle) + name;
    if (startOfSentence)
        result[0] = toupper(result[0]);
    return result;
}

string Alias::nameOnly() const
{
    return name;
}

bool Alias::isPlural() const
{
    return plural;
}

AliasList::AliasList(string name, bool isPlural)
{
    aliases.push_back(Alias(name, isPlural));
}

bool AliasList::add(string name, bool isPlural)
{
    for (vector<Alias>::const_iterator alias = aliases.begin(); alias != aliases.end(); alias++)
        if (alias->nameOnly() == name)            
            return false;
    aliases.push_back(Alias(name, isPlural));
    return true;
}

bool AliasList::del(string name)
{
    for (vector<Alias>::const_iterator alias = aliases.begin(); alias != aliases.end(); alias++)
        if (alias->nameOnly() == name)
        {
            aliases.erase(alias);
            return true;
        }
    return false;
}

bool AliasList::has(string name) const
{
    for (vector<Alias>::const_iterator alias = aliases.begin(); alias != aliases.end(); alias++)
        if (alias->isCompatible(name))
            return true;
    return false;
}

string AliasList::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases[0].generate(definiteArticle, startOfSentence);
}