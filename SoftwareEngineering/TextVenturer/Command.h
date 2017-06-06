#pragma once

// A command can contain <IDENTIFIER> to request parameters out of command
class Command
{
public:
    class Result
    {
    private: 
        const Command * command;
        dictionary parameters;
        bool success;

    public:
        Result(const Command & command, const std::string & input);
        Result();

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
    stringlist aliases; // all valid names for the command, can contain spaces
    taglist parameters; // list of parameters in command from constructor
    std::string * prepositions;

public:
    Command(FileStream & stream);
    Command();

    AddResult addAlias(std::string alias);
    bool delAlias(const std::string & alias); // probably not necessary, but adding it anyway

    void setPrepositions(std::string & prepositions);
    void resetPrepositions();

    std::string getName() const;
    stringlist getAliases() const;

    Result check(const std::string & input) const;

    static stringlist extractParameters(std::string cmd);
    static taglist paramsToSet(stringlist params);

    void save(FileStream & stream) const;
};

class EUnknownParameter : public Exception
{
public:
    EUnknownParameter(const std::string & parameter, const Command & command);
    EUnknownParameter(const std::string & parameter);
};