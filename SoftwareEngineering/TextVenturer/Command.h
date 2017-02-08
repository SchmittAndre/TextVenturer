#pragma once

// A command can contain <IDENTIFIER> to request parameters out of command
class Command
{
public:
    class Result
    {
        friend class Command;
    private: 
        dictionary parameters;
        bool success;

    public:
        explicit operator bool() const;   
        std::string operator[](const std::string & parameter) const;
        bool hasParam(const std::string & parameter) const;
    };

    enum AddResult {
        addIncompatible,
        addSuccess,
        addExists
    };

private:
    strings aliases; // all valid names for the command, can contain spaces
    strings parameters; // list of parameters in command from constructor
    std::string* prepositions;

public:
    Command(const std::string & cmd);
    
    AddResult addAlias(std::string alias);
    bool delAlias(const std::string & alias); // probably not necessary, but adding it anyway

    void setPrepositions(std::string* prepositions);

    std::string getName() const;
    strings getAliases() const;

    Result check(const std::string & input) const;

    static strings extractParameters(std::string cmd);
    static tags paramsToSet(strings params);

};

