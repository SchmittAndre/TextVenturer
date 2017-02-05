#pragma once

struct Alias
{
private:
    std::string name;
    bool plural;

public:
    Alias(std::string name, bool isPlural = false);
    bool startsWithVowel() const;
    bool isCompatible(std::string name) const;
    std::string getArticle(bool definiteArticle = false) const;
    std::string generate(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string nameOnly() const;
    bool isPlural() const;         
};

class AliasList
{
public:
    std::vector<Alias> aliases;

public:
    bool add(std::string name, bool isPlural = false);
    bool del(std::string name);
    bool has(std::string name) const;

    bool isNamePlural() const;

    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
};

