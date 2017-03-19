#pragma once

#include "AdventureAction.h"
#include "Command.h"
#include "CustomScript.h"

class Adventure;

class CustomAdventureAction : public AdventureAction
{
private:
    CustomScript::Script* script;
    bool overrideDefault;

public:
    CustomAdventureAction(Adventure * adventure, FileStream & stream);
    CustomAdventureAction(Adventure * adventure, std::string code, std::string title, bool overrideDefault = false);
    ~CustomAdventureAction();
   
    bool overrides() const;

    tags requiredParameters() const;
    bool compileSucceeded() const;
    bool run(const Command::Result & params = Command::Result());
    
    void save(FileStream & stream);
};