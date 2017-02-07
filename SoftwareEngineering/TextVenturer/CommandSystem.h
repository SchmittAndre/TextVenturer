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
    std::vector<CommandAction> commands;
    std::queue<std::string> commandQueue;

    std::set<std::string, sortStrRevLen> prepositions;
    std::string prepositionRegexString;

    void genPrepositions();

public:
    CommandSystem(Controler* controler, BaseAction* defaultAction);

    void add(Command* cmd, BaseAction* a);
    void del(Command* cmd);

    void addPreposition(std::string preposition);

    void sendCommand(const std::string &input);

    void update();

    bool processingCommand();

};

