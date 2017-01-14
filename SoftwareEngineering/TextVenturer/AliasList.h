#pragma once

struct Alias
{
private:
    string name;
    bool plural;

public:
    Alias(string name, bool isPlural = false);
    bool startsWithVowel() const;
    bool isCompatible(string name) const;
    string getArticle(bool definiteArticle = false) const;
    string generate(bool definiteArticle = false, bool startOfSentence = false) const;
    string nameOnly() const;
    bool isPlural() const;         
};

class AliasList
{
public:
    vector<Alias> aliases;

public:
    AliasList(string name, bool isPlural = false);

    bool add(string name, bool isPlural = false);
    bool del(string name);
    bool has(string name) const;

    string getName(bool definiteArticle = false, bool startOfSentence = false) const;
};

