#include "stdafx.h"

#include "Adventure.h"
#include "AdventureAction.h"
#include "AdventureObject.h"

#include "CustomAdventureAction.h"

CustomAdventureAction::CustomAdventureAction(Adventure * adventure, std::string code)
:   AdventureAction(adventure), script(this)
{
    script.loadFromString(code);
}

bool CustomAdventureAction::run(const Command::Result & params)
{         
    return script.run(params);
}

using namespace CustomScript;

Statement::Statement(CustomAdventureAction* action)
{
    next = NULL;
    this->action = action;
}

Statement::~Statement()
{
    delete next;
}

bool Statement::execute()
{
    if (next)
        return next->execute();
    else
        return true; 
}

void Statement::setNext(Statement * next)
{
    this->next = next;
}

void Statement::setParent(ControlStatement * parent)
{
    this->parent = parent;
}

ControlStatement * CustomScript::Statement::getParent()
{
    return parent;
}

IfStatement::IfStatement(CustomAdventureAction* action)
    : ControlStatement(action)
{
    thenPart = NULL;
    elsePart = NULL;
}

IfStatement::~IfStatement()
{
    delete thenPart;
    delete elsePart;
}

bool IfStatement::execute()
{
    bool success = true;
    if (condition->evaluate())
    {
        if (thenPart)
            success = thenPart->execute() && Statement::execute();
    }
    else
    {
        if (elsePart)
            success = elsePart->execute() && Statement::execute();
    }        
    return success && Statement::execute();
}

void IfStatement::setThenPart(Statement * thenPart)
{
    this->thenPart = thenPart;
    thenPart->setParent(this);
}

void IfStatement::setElsePart(Statement * elsePart)
{
    this->elsePart = elsePart;
    elsePart->setParent(this);
}

bool WhileStatement::execute()
{
    LoopStatement::preExecute();
    bool success = true;
    while (!breakOccured() && success && condition->evaluate())
        if (nextLooped)
            success = nextLooped->execute();
    return success && ControlStatement::execute();
}

bool DoWhileStatement::execute()
{
    LoopStatement::preExecute();
    bool success = true;
    do 
    {
        if (nextLooped)
            success = success && nextLooped->execute();
    } while (!breakOccured() && success && condition->evaluate());
    return success && ControlStatement::execute();
}

CustomScript::Script::Script(CustomAdventureAction * action)
{
    this->action = action;
}

bool Script::run(const Command::Result & params)
{
    this->params = (Command::Result*)&params;
    return root->execute();
}

bool Script::loadFromString(std::string code)
{
    // here comes crazy code
}

bool SkipStatement::execute()
{
    return false;
}

bool BreakStatement::execute()
{
    if (LoopStatement* loop = dynamic_cast<LoopStatement*>(getParent()))
        loop->doBreak();
    return true;
}

bool ContinueStatement::execute()
{
    return true;
}

bool CustomScript::LoopStatement::breakOccured()
{
    return breakFlag;
}

void LoopStatement::doBreak()
{
    breakFlag = true;
}

void LoopStatement::preExecute()
{
    breakFlag = false;
}

CustomScript::Expression::Expression(CustomAdventureAction * action)
{
    this->action = action;
}

CustomAdventureAction * CustomScript::Expression::getAction()
{
    return action;
}

AdventureObject * CustomScript::ParamExpression::evaluate()
{
    return getAction()->getAdventure()->findObjectByAlias(param);
}
