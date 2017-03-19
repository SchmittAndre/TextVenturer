#pragma once

struct Alias
{
private:
    std::string name;
    bool plural;

public:
    Alias(FileStream & stream);
    Alias(std::string name, bool isPlural = false);
    bool startsWithVowel() const;
    bool isCompatible(std::string name) const;
    std::string getArticle(bool definiteArticle = false) const;
    std::string generate(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string nameOnly(bool startOfSentence = false) const;
    bool isPlural() const; 

    void save(FileStream & stream);
};

class AliasList
{
private:
    std::vector<Alias> aliases;

public:
    bool add(std::string name, bool isPlural = false);
    bool del(std::string name);
    bool has(std::string name) const;

    bool isNamePlural() const;

    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getNameOnly(bool startOfSentence = false) const;

    std::string genRegex() const;

    void save(FileStream & stream);
    void load(FileStream & stream);
};

