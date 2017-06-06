#pragma once

#include "Command.h"

struct AdventureLoadHelp;
struct AdventureSaveHelp;
class AdventureAction;

struct CommandAction
{
    Command * command;
    AdventureAction * action;

    CommandAction(Command & command, AdventureAction & action);
};

class CommandArray
{
private:
    bool referenced;
    std::vector<CommandAction> commands;

public:
    CommandArray(bool referenced = false);
    CommandArray(FileStream & stream, AdventureLoadHelp & help, bool referenced = false);
    ~CommandArray();
   
    void add(Command & cmd, AdventureAction & action);
    void del(Command & cmd);

    bool sendCommand(std::string input);

    std::vector<CommandAction>::iterator begin();
    std::vector<CommandAction>::iterator end();

    void save(FileStream & stream, AdventureSaveHelp & help) const;
};

class CommandSystem
{
private:
    struct sortStrRevLen
    {
        bool operator() (const std::string & a, const std::string & b) const
        {
            return a.size() == b.size() ? a < b : a.size() > b.size();
        }
    };

    struct ParamAction {
        AdventureAction & action;
        Command::Result params;

        ParamAction(AdventureAction & action, Command::Result params);
    };

    AdventureAction & defaultAction;
    CommandArray commands;
    std::queue<std::string> commandQueue;

    ref_vector<CommandArray> commandArrays;

    std::set<std::string, sortStrRevLen> prepositions;
    std::string prepositionRegexString;

    void genPrepositions();

public:
    CommandSystem(AdventureAction & defaultAction);
    
    void add(Command & cmd, AdventureAction & action);
    void del(Command & cmd);
    
    void add(CommandArray & commandArray);
    void del(CommandArray & commandArray);

    void addPreposition(std::string preposition);

    void sendCommand(const std::string & input);

    void update();

    bool processingCommand() const;

    void save(FileStream & stream, AdventureSaveHelp & help) const;
    void load(FileStream & stream, AdventureLoadHelp & help);
};

class ECommandDoesNotExist : public Exception
{
public:
    ECommandDoesNotExist(const Command & cmd);
};

class ECommandMissingParameters : public Exception
{
public:
    ECommandMissingParameters(const Command & cmd, const std::string & params);
};

class ECommandArrayDoesNotExist : public Exception
{
public:
    ECommandArrayDoesNotExist();
};
