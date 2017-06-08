#include "stdafx.h"

#include "AliasList.h"

Alias::Alias(FileStream & stream)
{
    stream.read(plural);
    stream.read(name);
}

Alias::Alias(std::string name, bool isPlural)
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
    
    if (name.size() == 1)
        return true;

    char firstLow = tolower(name[0]);
    if (islower(name[1]))
    {
        return 
            firstLow == 'a' ||
            firstLow == 'e' ||
            firstLow == 'i' ||
            firstLow == 'o' ||
            firstLow == 'u';
    }
    else
    {
        // seperated, because U as a single letter is not a vowel
        return
            firstLow == 'a' ||
            firstLow == 'e' ||
            firstLow == 'f' ||
            firstLow == 'h' ||
            firstLow == 'i' ||
            firstLow == 'l' ||
            firstLow == 'm' ||
            firstLow == 'n' ||
            firstLow == 'o' ||
            firstLow == 'r' ||
            firstLow == 's' ||
            firstLow == 'x';
    }
}

bool Alias::isCompatible(std::string name) const
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

std::string Alias::getArticle(bool definiteArticle) const
{
    return definiteArticle ? "the " : (plural ? "" : startsWithVowel() ? "an " : "a ");
}          

std::string Alias::generate(bool definiteArticle, bool startOfSentence) const
{
    std::string result = getArticle(definiteArticle) + name;
    if (startOfSentence)
        result[0] = toupper(result[0]);
    return result;
}

std::string Alias::nameOnly(bool startOfSentence) const
{
    std::string result = name;
    if (startOfSentence)
        result[0] = toupper(result[0]);
    return result;
}

bool Alias::isPlural() const
{
    return plural;
}

void Alias::save(FileStream & stream)
{
    stream.write(plural);
    stream.write(name);
}

AliasList::AliasList(FileStream & stream)
{                                           
    load(stream);
}

AliasList::AliasList()
{
}

bool AliasList::add(std::string name, bool isPlural)
{
    for (std::vector<Alias>::const_iterator alias = aliases.begin(); alias != aliases.end(); alias++)
        if (alias->nameOnly() == name)            
            return false;
    aliases.push_back(Alias(name, isPlural));
    return true;
}

bool AliasList::del(std::string name)
{
    for (std::vector<Alias>::const_iterator alias = aliases.begin(); alias != aliases.end(); alias++)
        if (alias->nameOnly() == name)
        {
            aliases.erase(alias);
            return true;
        }
    return false;
}

bool AliasList::has(std::string name) const
{
    for (std::vector<Alias>::const_iterator alias = aliases.begin(); alias != aliases.end(); alias++)
        if (alias->isCompatible(name))
            return true;
    return false;
}

bool AliasList::isNamePlural() const
{
    return !aliases.empty() && aliases[0].isPlural();
}

bool AliasList::empty()
{
    return aliases.empty();
}

std::string AliasList::getName(bool definiteArticle, bool startOfSentence) const
{
    if (aliases.size())
        return aliases[0].generate(definiteArticle, startOfSentence);
    else
        return "Missing name!";
}

std::string AliasList::getNameOnly(bool startOfSentence) const
{
    if (aliases.size())
        return aliases[0].nameOnly(startOfSentence);
    else
        return "Missing name!";
}

std::string AliasList::genRegex() const
{
    std::string result;
    result += "(?:";
    for (Alias alias : aliases)
        result += alias.nameOnly() + "|";
    result.pop_back();
    result += ")";
    return result;
}

void AliasList::save(FileStream & stream) const
{
    stream.write(static_cast<UINT>(aliases.size()));
    for (Alias alias : aliases)
        alias.save(stream);
}

void AliasList::load(FileStream & stream)
{
    UINT size = stream.readUInt();
    for (UINT i = 0; i < size; i++)
        aliases.push_back(Alias(stream));
}
