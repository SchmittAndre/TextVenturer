#include "stdafx.h"

#include "Adventure.h"
#include "AdventureAction.h"
#include "AdventureObject.h"

#include "CustomAdventureAction.h"

using namespace CustomScript;

StringBounds::StringBounds(const std::string & text)
    : begin(text.cbegin())
    , end(text.cend())
{
}

StringBounds::StringBounds(const std::string::const_iterator begin, const std::string::const_iterator end)
    : begin(begin)
    , end(end)
{
}

StringBounds::StringBounds(const std::ssub_match & submatch)
    : begin(submatch.first)
    , end(submatch.second)
{
}

ExpressionParseData::ExpressionParseData(StringBounds & bounds, Script * script)
    : bounds(bounds)
{
    this->script = script;
}

StatementParseData::StatementParseData(StringBounds & bounds, Script * script, ControlStatement * parent)
    : bounds(bounds)
{
    this->script = script;
    this->parent = parent;
}

// ObjectExpression                     

const ObjectExpression::TryParseFunc ObjectExpression::TryParseList[] = {
    IdentExpression::TryParse,
    ParamExpression::TryParse
};

ObjectExpression * ObjectExpression::TryParse(ExpressionParseData & data)
{
    ObjectExpression* expr;
    for (TryParseFunc func : TryParseList)
    {
        if (func(data, expr))
        {
            if (expr)
            {
                return expr;
            }
        }
        else
        {
            break;
        }
    }
    return NULL;
}

// BoolExpression

const BoolExpression::TryParseFunc BoolExpression::TryParseList[] = {
    ConstBoolExpression::TryParse,
    ParamIsIdentExpression::TryParse
};

BoolExpression * BoolExpression::TryParse(ExpressionParseData & data)
{
    BoolExpression* expr;
    for (TryParseFunc func : TryParseList)
    {
        if (func(data, expr))
        {
            if (expr)
            {
                return expr;
            }
        }
        else
        {
            break;
        }
    }
    return NULL;
}

// StringExpression 

const StringExpression::TryParseFunc StringExpression::TryParseList[] = {
    ObjectToStringExpression::TryParse,
    StringConcatExpression::TryParse,
    ConstStringExpression::TryParse
};

StringExpression * StringExpression::TryParse(ExpressionParseData & data)
{
    StringExpression* expr;
    for (TryParseFunc func : TryParseList)
    {
        if (func(data, expr))
        {
            if (expr)
            {
                return expr;
            }
        }
        else
        {
            break;
        }
    }
    return NULL;
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

bool CustomScript::IdentExpression::TryParse(ExpressionParseData & data, ObjectExpression *& expr)
{
    // TODO: IdentExpression::TryParse
    expr = NULL;
    return true;
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

bool CustomScript::ParamExpression::TryParse(ExpressionParseData & data, ObjectExpression *& expr)
{
    // TODO: ParamExpression::TryParse
    expr = NULL;
    return true;
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

bool CustomScript::ParamIsIdentExpression::TryParse(ExpressionParseData & data, BoolExpression *& expr)
{
    // TODO: ParamIsIdentExpression::TryParse
    expr = NULL;
    return true;
}

// ConstBoolExpression

void ConstBoolExpression::setValue(bool value)
{
    this->value = value;
}

bool ConstBoolExpression::evaluate()
{
    return value;
}

bool CustomScript::ConstBoolExpression::TryParse(ExpressionParseData & data, BoolExpression *& expr)
{
    // TODO: ConstBoolExpression::TryParse
    expr = NULL;
    return true;
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
        break;
    default:
        return "[unknown nameing type]";
    }
}

bool CustomScript::ObjectToStringExpression::TryParse(ExpressionParseData & data, StringExpression *& expr)
{               
    // TODO: ObjectToStringExpression::TryParse
    expr = NULL;
    return true;
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

bool CustomScript::ConstStringExpression::TryParse(ExpressionParseData & data, StringExpression *& expr)
{
    // TODO: ConstStringExpression::TryParse
    const static std::regex stringExp("\"" + any + "\"");
    expr = NULL;
    return true;
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

bool CustomScript::StringConcatExpression::TryParse(ExpressionParseData & data, StringExpression *& expr)
{               
    // TODO: StringConcatExpression::TryParse
    expr = NULL;
    return true;
}

// Statement

const Statement::TryParseFunc Statement::TryParseList[] = {
    IfStatement::TryParse,
    WhileStatement::TryParse,
    DoWhileStatement::TryParse,
    BreakStatement::TryParse,
    ContinueStatement::TryParse,
    SkipStatement::TryParse,
    WriteStatement::TryParse
};

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

void Statement::setScript(Script * script)
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

bool Statement::parse(StatementParseData& data)
{
    for (TryParseFunc func : TryParseList)
    {        
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

bool Statement::check_regex(StringBounds & bounds, std::smatch & matches, const std::regex & exp)
{
    return std::regex_search(bounds.begin, bounds.end, matches, exp, std::regex_constants::match_continuous);
}

void Statement::skipWhitespaces(StringBounds & bounds)
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

bool IfStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: IfStatement::TryParse
    return true;

    static const std::regex expIf("if");
    static const std::regex expThen("(" + any + ")" + ws1 + "then");
    static const std::regex expElse("else");
    static const std::regex expEnd("end");

    std::smatch matches;
    if (!check_regex(data.bounds, matches, expIf))
        return true;

    data.bounds.begin += matches[0].length();
    skipWhitespaces(data.bounds);

    if (!check_regex(data.bounds, matches, expThen))
    {
        data.script->error("missing then");
        return false;
    }                 

    BoolExpression* condition;
    condition = new ConstBoolExpression(data.script);
    ((ConstBoolExpression*)condition)->setValue(true);
    // TODO: IfStatement parse condition
    //if (!BoolExpression::TryParse(StringBounds(matches[1]), condition))
    //{
    //   
    //}                   

    data.bounds.begin += matches[0].length();
    skipWhitespaces(data.bounds);

    IfStatement* typed = new IfStatement();
    stmt = typed;

    typed->setScript(data.script);
    typed->setParent(data.parent);  
    typed->setCondition(condition);
        
    Statement* thenPart = new Statement();
    typed->setThenPart(thenPart);
    thenPart->parse(StatementParseData(data.bounds, data.script, typed));

    if (check_regex(data.bounds, matches, expElse))
    {
    
    }
    
    if (check_regex(data.bounds, matches, expEnd))
    {
    
    }

    return true;
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

bool WhileStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: WhileStatement::TryParse
    return true;
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

bool DoWhileStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: DoWhileStatement::TryParse    
    return true;
}

// BreakStatement

bool BreakStatement::execute()
{
    if (LoopStatement* loop = dynamic_cast<LoopStatement*>(getParent()))
        loop->doBreak();
    return true;
}

bool BreakStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: BreakStatement::TryParse    
    return true;
}

// ContinueStatement

bool ContinueStatement::execute()
{
    return true;
}

bool ContinueStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: ContinueStatement::TryParse
    return true;
}

// SkipStatement

bool SkipStatement::execute()
{
    return false;
}

bool SkipStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: SkipStatement::TryParse
    return true;
}

// WriteStatement

WriteStatement::WriteStatement()
{
    stringExp = NULL;
}

WriteStatement::~WriteStatement()
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

bool WriteStatement::TryParse(StatementParseData & data, Statement *& stmt)
{
    // TODO: WriteStatement::TryParse
    const static std::regex expWrite("write");
    
    std::smatch matches;
    if (!check_regex(data.bounds, matches, expWrite))
        return true;

    data.bounds.begin += matches[0].length();
    skipWhitespaces(data.bounds);

    StringExpression* expr = StringExpression::TryParse(ExpressionParseData(data.bounds, data.script));
    if (!expr)
        return false;        

    return true;
}

// Script

Script::Script(CustomAdventureAction* action, std::string code, std::string title)
    : parseData(StringBounds(code), this)
{
    this->action = action;
    this->title = title;

    codeBegin = parseData.bounds.begin;
    root.setScript(this);
    success = root.parse(parseData);
}

bool Script::run(const Command::Result & params)
{
    this->params = &params;
    return root.execute();
}

const Command::Result & Script::getParams() const
{
    return *params;
}

CustomAdventureAction * Script::getAction() const
{
    return action;
}

bool Script::succeeded()
{
    return success;
}

void Script::error(std::string message)
{
    size_t line = std::count(codeBegin, parseData.bounds.begin, '\n');
    size_t column = 42;
    ErrorDialog("CustomScript Error", 
                "Error in script " + title + 
                " at line " + std::to_string(line) + 
                " column " + std::to_string(column) +
                ":\n" + message);
}

// CustomAdventureAction

CustomAdventureAction::CustomAdventureAction(Adventure * adventure, std::string code, std::string title)
    : AdventureAction(adventure), script(this, code, title)
{
}

bool CustomAdventureAction::run(const Command::Result & params)
{
    return script.run(params);
}
