#pragma once

#include "AdventureAction.h"
#include "Command.h"
#include "CustomScript.h"
#include "AdventureStructure.h"

class Adventure;

class CustomAdventureAction : public AdventureAction
{
private:
    CustomScript::Script script;
    bool overrideDefault;

public:
    CustomAdventureAction(FileStream & stream, Adventure & adventure);
    CustomAdventureAction(Adventure & adventure, std::string code, std::string title, AdventureStructure::LineInfo lineinfo, bool overrideDefault = false);
    
    bool overrides() const;

    taglist requiredParameters() const;
    bool compileSucceeded() const;
    bool run(const Command::Result & params = Command::Result());
    
    void save(FileStream & stream) const;

    static void saveConditional(FileStream & stream, CustomAdventureAction * action);
    static CustomAdventureAction * loadConditional(FileStream & stream, Adventure & adventure);
};