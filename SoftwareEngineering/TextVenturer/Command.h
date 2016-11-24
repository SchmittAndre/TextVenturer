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
        string operator[](string parameter);

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
    Command(string cmd);
    
    AddResult addAlias(string alias);
    bool delAlias(string alias); // probably not necessary, but adding it anyway

    string getName();
    strings getAliases();

    Result check(string input);

    static strings extractParameters(string cmd);

};

