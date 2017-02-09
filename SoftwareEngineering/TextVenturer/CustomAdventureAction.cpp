#include "stdafx.h"

#include "Adventure.h"
#include "AdventureAction.h"
#include "AdventureObject.h"

#include "CustomAdventureAction.h"

// Custom AdventureAction

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

CustomScript::StringBounds::StringBounds(const std::string & text)
    : begin(text.cbegin())
    , end(text.cend())
{
}

CustomScript::StringBounds::StringBounds(const std::string::const_iterator begin, const std::string::const_iterator end)
    : begin(begin)
    , end(end)
{
}

CustomScript::StringBounds::StringBounds(const std::ssub_match & submatch)
    : begin(submatch.first)
    , end(submatch.second)
{
}

CustomScript::StatementParseData::StatementParseData(const StringBounds & bounds, Script * script, ControlStatement * parent)
    : bounds(bounds)
{
    this->script = script;
    this->parent = parent;
}


// IdentExpression

void IdentExpression::setIdent(const std::string & ident)
{
    this->ident = ident;
}

AdventureObject * IdentExpression::evaluate()
{
    return getAction()->getAdventure()->findObjectByName(ident);
}

// ParamExpression

void ParamExpression::setParam(const std::string & param)
{
    this->param = param;
}

AdventureObject * ParamExpression::evaluate()
{
    return getAction()->getAdventure()->findObjectByAlias(getParams()[param]);
}

// ParamIsIdentExpression

void ParamIsIdentExpression::setParamExpression(ParamExpression * paramExp)
{
    this->paramExp = paramExp;
}

void ParamIsIdentExpression::setIdentExpression(IdentExpression * identExp)
{
    this->identExp = identExp;
}

bool ParamIsIdentExpression::evaluate()
{
    return paramExp->evaluate() == identExp->evaluate();
}

// ConstBoolExpression

void CustomScript::ConstBoolExpression::setValue(bool value)
{
    this->value = value;
}

bool CustomScript::ConstBoolExpression::evaluate()
{
    return value;
}

// ObjectToStringExpression

void ObjectToStringExpression::setObjectExp(ObjectExpression * objectExp)
{
    this->objectExp = objectExp;
}

void ObjectToStringExpression::setStartOfSentence(bool startOfSentence)
{
    this->startOfSentence = startOfSentence;
}

void ObjectToStringExpression::setGenerateType(GenerateType type)
{
    this->type = type;
}

std::string ObjectToStringExpression::evaluate()
{
    switch (type)
    {
    case gtArticleFromPlayer:
        return objectExp->evaluate()->getName(getAction()->getPlayer(), startOfSentence);
        break;
    case gtDefiniteArticle:
        return objectExp->evaluate()->getName(true, startOfSentence);
        break;
    case gtIndefiniteAricle:
        return objectExp->evaluate()->getName(false, startOfSentence);
        break;
    case gtNameOnly:
        return objectExp->evaluate()->getNameOnly(startOfSentence);
    }
}

// ConstStringExpression

void ConstStringExpression::setText(const std::string & text)
{
    this->text = text;
}

std::string ConstStringExpression::evaluate()
{
    return text;
}

// StringConcatExpression

void StringConcatExpression::add(StringExpression * stringExp)
{
    this->stringExpList.push_back(stringExp);
}

std::string StringConcatExpression::evaluate()
{
    std::string result;
    for (StringExpression* exp : stringExpList)
        result += exp->evaluate();
    return result;
}

// Statement

const std::string Statement::ws = "[ \n\r\t]";
const std::string Statement::ws0 = ws + "*";
const std::string Statement::ws1 = ws + "+";
const std::string Statement::any = "[^]+?";
const std::string Statement::capture_any = ws1 + "(" + any + ")" + ws1;

Statement::Statement()
{
    next = NULL;
}

Statement::~Statement()
{
    delete next;
}

void Statement::setNext(Statement * next)
{
    this->next = next;
}

void Statement::setParent(ControlStatement * parent)
{
    this->parent = parent;
}

void CustomScript::Statement::setScript(Script * script)
{
    this->script = script;
}

ControlStatement * Statement::getParent()
{
    return parent;
}

const Command::Result & Statement::getParams() const
{
    return script->getParams();
}

CustomAdventureAction * Statement::getAction() const
{
    return script->getAction();
}

bool Statement::execute()
{
    if (next)
        return next->execute();
    else
        return true;
}

bool CustomScript::Statement::parse(StatementParseData& data)
{
    for (StatementTryParseFunc func : StatementTryParseList)
    {
        skipWhitespaces(data.bounds);
        
        if (func(data, next))
        {
            if (next)
            {
                return next->parse(data);
            }
        }
        else
        {
            break;
        }
    }
    return false;
}

bool CustomScript::Statement::check_regex(StringBounds & bounds, std::smatch & matches, const std::regex & exp)
{
    return std::regex_search(bounds.begin, bounds.end, matches, exp, std::regex_constants::match_continuous);
}

void CustomScript::Statement::skipWhitespaces(StringBounds & bounds)
{
    static const std::regex whitespaces(ws0);
    std::smatch matches;
    if (check_regex(bounds, matches, whitespaces))
        bounds.begin += matches[0].length();
}

// ControlStatement

bool ControlStatement::evaluateCondition()
{
    return condition->evaluate();
}

ControlStatement::ControlStatement()
{
    condition = NULL;
}

ControlStatement::~ControlStatement()
{
    delete condition;
}

void ControlStatement::setCondition(BoolExpression * condition)
{
    this->condition = condition;
}

// IfStatement

IfStatement::IfStatement()
{
    thenPart = NULL;
    elsePart = NULL;
}

IfStatement::~IfStatement()
{
    delete thenPart;
    delete elsePart;
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

bool IfStatement::execute()
{
    bool success = true;
    if (evaluateCondition())
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

bool CustomScript::IfStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: IfStatement::TryParse
    static const std::regex expIf("if" + capture_any + "then" + capture_any + "end");
    static const std::regex expElse(capture_any + "else" + capture_any);

    std::smatch matches;
    if (check_regex(data.bounds, matches, expIf))
    {
        IfStatement* typed = new IfStatement();
        stmt = typed;

        typed->setScript(data.script);
        typed->setParent(data.parent);
        
        BoolExpression* condition;
        // TODO: IfStatement parse condition
        //if (!BoolExpression::TryParse(StringBounds(matches[1]), condition))
        //{
        //   
        //}
        condition = new ConstBoolExpression(data.script);
        ((ConstBoolExpression*)condition)->setValue(true);

        typed->setCondition(condition);
        
        std::smatch matchesElse;
        if (check_regex(data.bounds, matchesElse, expElse))
        {                                                         
            Statement* thenPart = new Statement();
            typed->setThenPart(thenPart);
            thenPart->parse(StatementParseData(StringBounds(matchesElse[1]), data.script, typed));

            Statement* elsePart = new Statement();
            typed->setElsePart(elsePart);
            elsePart->parse(StatementParseData(StringBounds(matchesElse[2]), data.script, typed));
        }
        else
        {                                              
            Statement* thenPart = new Statement();
            typed->setThenPart(thenPart);
            thenPart->parse(StatementParseData(StringBounds(matches[2]), data.script, typed));             
        }
    }
}

// LoopStatement

bool LoopStatement::executeLoopPart()
{
    return loopPart && loopPart->execute() || !loopPart;
}

bool LoopStatement::breakOccured()
{
    return breakFlag;
}

void LoopStatement::preExecute()
{
    breakFlag = false;
}

LoopStatement::LoopStatement()
{
    loopPart = NULL;
}

LoopStatement::~LoopStatement()
{
    delete loopPart;
}

void LoopStatement::setLoopPart(Statement * loopPart)
{
    this->loopPart = loopPart;
}

void LoopStatement::doBreak()
{
    breakFlag = true;
}

// WhileStatement

bool WhileStatement::execute()
{
    LoopStatement::preExecute();
    bool success = true;
    while (!breakOccured() && success && evaluateCondition())
        success = executeLoopPart();
    return success && ControlStatement::execute();
}

bool CustomScript::WhileStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: WhileStatement::TryParse
}

// DoWhileStatement

bool DoWhileStatement::execute()
{
    LoopStatement::preExecute();
    bool success;
    do 
    {
        success = executeLoopPart();
    } while (!breakOccured() && success && evaluateCondition());
    return success && ControlStatement::execute();
}

bool CustomScript::DoWhileStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: DoWhileStatement::TryParse
}

// BreakStatement

bool BreakStatement::execute()
{
    if (LoopStatement* loop = dynamic_cast<LoopStatement*>(getParent()))
        loop->doBreak();
    return true;
}

bool CustomScript::BreakStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: BreakStatement::TryParse
}

// ContinueStatement

bool ContinueStatement::execute()
{
    return true;
}

bool CustomScript::ContinueStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: ContinueStatement::TryParse
}

// SkipStatement

bool SkipStatement::execute()
{
    return false;
}

bool CustomScript::SkipStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: SkipStatement::TryParse
}

// WriteStatement

CustomScript::WriteStatement::WriteStatement()
{
    stringExp = NULL;
}

CustomScript::WriteStatement::~WriteStatement()
{
    delete stringExp;
}

void WriteStatement::setStringExpression(StringExpression * stringExp)
{
    this->stringExp = stringExp;
}

bool WriteStatement::execute()
{
    getAction()->write(stringExp->evaluate());
    return Statement::execute();
}

bool CustomScript::WriteStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: WriteStatement::TryParse
}

// Script

Script::Script(CustomAdventureAction * action)
{
    this->action = action;
    root.setScript(this);
}

bool Script::run(const Command::Result & params)
{
    this->params = &params;
    return root.execute();
}

bool Script::loadFromString(std::string code)
{
    return root.parse(StatementParseData(StringBounds(code), this));
}

const Command::Result & Script::getParams() const
{
    return *params;
}

CustomAdventureAction * Script::getAction() const
{
    return action;
}
