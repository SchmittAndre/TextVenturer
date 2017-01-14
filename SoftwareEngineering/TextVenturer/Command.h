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
        string operator[](const string & parameter) const;
        bool hasParam(const string & parameter) const;
    };

    enum AddResult {
        addIncompatible,
        addSuccess,
        addExists
    };

private:
    strings aliases; // all valid names for the command, can contain spaces

    tags parameters; // list of parameters in command from constructor

public:
    Command(const string & cmd);
    
    AddResult addAlias(const string & alias);
    bool delAlias(const string & alias); // probably not necessary, but adding it anyway

    string getName() const;
    strings getAliases() const;

    Result check(const string & input) const;

    static strings extractParameters(string cmd);

};

