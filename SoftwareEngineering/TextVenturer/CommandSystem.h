#pragma once

#include "Command.h"

struct AdventureLoadHelp;
struct AdventureSaveHelp;
class AdventureAction;
class Controler;
class Adventure;

struct CommandAction
{
    Command & command;
    AdventureAction & action;

    CommandAction(Command & command, AdventureAction & action);
};

class CommandArray
{
private:
    std::vector<CommandAction> commands;

public:
    CommandArray();
    CommandArray(FileStream & stream, AdventureLoadHelp & help);

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

        ParamAction(AdventureAction & action, Command::Result params = Command::Result());
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
    CommandSystem(FileStream & stream, AdventureLoadHelp & help, AdventureAction & defaultAction);
    
    void add(Command & cmd, AdventureAction & action);
    void del(Command & cmd);
    
    void add(CommandArray & commandArray);
    void del(CommandArray & commandArray);

    void addPreposition(std::string preposition);

    void sendCommand(const std::string & input);

    void update();

    bool processingCommand() const;

    void save(FileStream & stream, AdventureSaveHelp & help) const;
};
