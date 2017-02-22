#pragma once

#include "Command.h"

class BaseAction;
class Controler;

struct CommandAction
{
    Command* command;
    BaseAction* action;

    CommandAction(Command* cmd, BaseAction* action);
};

class CommandArray
{
private:
    bool referenced;
    std::vector<CommandAction> commands;
public:
    CommandArray(bool referenced = false);
    ~CommandArray();

    void add(Command* cmd, BaseAction* action);
    void del(Command* cmd);

    bool sendCommand(std::string input);

    std::vector<CommandAction>::iterator begin();
    std::vector<CommandAction>::iterator end();
};

class CommandSystem
{
private:
    struct sortStrRevLen
    {
        bool operator() (const std::string& a, const std::string& b) const
        {
            return a.size() == b.size() ? a < b : a.size() > b.size();
        }
    };

    struct ParamAction {
        BaseAction* action;
        Command::Result params;

        ParamAction(BaseAction* action, Command::Result params = Command::Result());
    };

    BaseAction* defaultAction;
    CommandArray commands;
    std::queue<std::string> commandQueue;

    std::vector<CommandArray*> commandArrays;

    std::set<std::string, sortStrRevLen> prepositions;
    std::string prepositionRegexString;

    void genPrepositions();

public:
    CommandSystem(Controler* controler, BaseAction* defaultAction);

    void add(Command* cmd, BaseAction* action);
    void del(Command* cmd);
    
    void add(CommandArray* commandArray);
    void del(CommandArray* commandArray);

    void addPreposition(std::string preposition);

    void sendCommand(const std::string &input);

    void update();

    bool processingCommand();

};

