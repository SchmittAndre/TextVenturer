#include "stdafx.h"

#include "Adventure.h"
#include "AdventureAction.h"
#include "AdventureObject.h"

#include "CustomAdventureAction.h"

using namespace CustomScript;

void CustomScript::StringBounds::advance(size_t amount, bool seekNext)
{
    begin += amount;
    if (seekNext)
        skipWhitespaces(*this);
}

StringBounds::StringBounds(const std::string & text)
    : begin(text.cbegin())
    , end(text.cend())
{
}

StringBounds::StringBounds(std::string::const_iterator begin, std::string::const_iterator end)
    : begin(begin)
    , end(end)
{
}

StringBounds::StringBounds(const std::ssub_match & submatch)
    : begin(submatch.first)
    , end(submatch.second)
{
}

ParseData::ParseData(StringBounds bounds, Script * script, ControlStatement * parent)
    : bounds(bounds)
{
    this->script = script;
    this->parent = parent;
}

// ObjectExpression                     

const ObjectExpression::TryParseFunc ObjectExpression::TryParseList[] = {
    IdentExpression::TryParse
};

ObjectExpression * ObjectExpression::TryParse(ParseData & data)
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
    ParamIsIdentExpression::TryParse,
    ConstBoolExpression::TryParse
};

BoolExpression * BoolExpression::TryParse(ParseData & data)
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
    ParamExpression::TryParse,
    ConstStringExpression::TryParse
};

// IdentExpression

AdventureObject * IdentExpression::evaluate()
{
    return getAction()->getAdventure()->findObjectByName(identifier);
}

bool CustomScript::IdentExpression::TryParse(ParseData & data, ObjectExpression *& expr)
{
    static const std::regex identExp(":(" + ident + ")");
    expr = NULL;
    std::smatch matches;
    if (!check_regex(data.bounds, matches, identExp))
        return true;   
    data.bounds.advance(matches[0].length());
    IdentExpression* typed = new IdentExpression(data.script);
    expr = typed;
    typed->identifier = matches[1];
    return true;
}

// ParamIsIdentExpression

bool ParamIsIdentExpression::evaluate()
{
    return getAction()->getAdventure()->findObjectByAlias(paramExp->evaluate()) == identExp->evaluate();
}

bool CustomScript::ParamIsIdentExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex isExp("is");
    
    StringExpression* param;
    ObjectExpression* ident;

    expr = NULL;

    if (!ParamExpression::TryParse(data, param))
        return true;

    std::smatch matches;
    if (!check_regex(data.bounds, matches, isExp))
        return true;
    data.bounds.advance(matches[0].length());
    
    if (!IdentExpression::TryParse(data, ident))
        return true;

    ParamIsIdentExpression* typed = new ParamIsIdentExpression(data.script);
    expr = typed;
    typed->paramExp = (ParamExpression*)param;
    typed->identExp = (IdentExpression*)ident;
    return true;
}

// ConstBoolExpression

bool ConstBoolExpression::evaluate()
{
    return value;
}

bool CustomScript::ConstBoolExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex boolExp("(true|false)");
    expr = NULL;
    std::smatch matches;
    if (!check_regex(data.bounds, matches, boolExp))
        return true;
    data.bounds.advance(matches[0].length());
    ConstBoolExpression* typed = new ConstBoolExpression(data.script);
    expr = typed;
    typed->value = matches[1] == "true";
    return true;
}

// ObjectToStringExpression

std::string ObjectToStringExpression::evaluate()
{
    AdventureObject* obj = objectExp->evaluate();
    if (!obj)
        return "[unknown object]";

    switch (type)
    {
    case gtArticleFromPlayer:
        return obj->getName(getAction()->getPlayer(), startOfSentence);
        break;
    case gtDefiniteArticle:
        return obj->getName(true, startOfSentence);
        break;
    case gtIndefiniteAricle:
        return obj->getName(false, startOfSentence);
        break;
    case gtNameOnly:
        return obj->getNameOnly(startOfSentence);
        break;
    default:
        return "[unknown nameing type]";
    }
}

bool CustomScript::ObjectToStringExpression::TryParse(ParseData & data, StringExpression *& expr)
{               
    static const std::regex typeExp("\\[([^]+?)\\]");

    expr = NULL;
    ObjectExpression* objectExpression = ObjectExpression::TryParse(data);
    if (!objectExpression)
        return true;
    
    ObjectToStringExpression* typed = new ObjectToStringExpression(data.script);
    expr = typed;
    typed->objectExp = objectExpression;

    std::smatch matches;
    if (check_regex(data.bounds, matches, typeExp))
    {
        static const std::string typeHelp =
            "\n  Valid chars are:"
            "\n    [p] Definite article if player knows object (default)"
            "\n    [d] Force definite article"
            "\n    [i] Force indefinite article"
            "\n    [n] name only"
            "\n  char is uppercase -> first char of name captial";
        if (matches[1].length() > 1)
        {
            data.script->error("Identifier-Types are only single chars!" + typeHelp);
            return false;
        }
        data.bounds.advance(matches[0].length());
        char c = matches[1].str()[0];
        typed->startOfSentence = isupper(c) != 0;
        switch (tolower(c))
        {
        case 'p':
            typed->type = gtArticleFromPlayer;
            break;
        case 'd':
            typed->type = gtDefiniteArticle;
            break;
        case 'i':
            typed->type = gtIndefiniteAricle;
            break;
        case 'n':
            typed->type = gtNameOnly;
            break;
        default:
            data.script->error("Unknown Identifier-Type \"" + matches[1].str() + "\"!" + typeHelp);
            return false;
        }   
    }
    else
    {
        typed->startOfSentence = false;
        typed->type = gtArticleFromPlayer;
    }
    return true;
}

// ConstStringExpression

std::string ConstStringExpression::evaluate()
{
    return text;
}

bool ConstStringExpression::TryParse(ParseData & data, StringExpression *& expr)
{
    const static std::regex stringExp("\"((?:(?:\\\\[^])|[^\\\\\"])*)\"");
    const static std::regex escapeExp("\\\\([^])");
    expr = NULL;
    std::smatch matches;
    if (!check_regex(data.bounds, matches, stringExp))
        return true;
    ConstStringExpression* typed = new ConstStringExpression(data.script);
    expr = typed;
    typed->text = std::regex_replace(matches[1].str(), escapeExp, "$1");
    data.bounds.advance(matches[0].length());
    return true;
}

// StringConcatExpression

std::string StringConcatExpression::evaluate()
{
    std::string result;
    for (StringExpression* exp : stringExpList)
        result += exp->evaluate();
    return result;
}

StringConcatExpression* CustomScript::StringConcatExpression::TryParse(ParseData & data)
{               
    StringConcatExpression* result = new StringConcatExpression(data.script);
    StringExpression* sub;
    bool foundFirst = false;
    while (true)
    {
        bool found = false;
        for (TryParseFunc func : TryParseList)
        {
            if (func(data, sub))
            {
                if (sub)
                {
                    result->stringExpList.push_back(sub);
                    found = true;
                    foundFirst = true;
                    break;
                }
            }
            else
            {
                delete result;
                return NULL;
            }
        }
        if (!foundFirst)
        {
            delete result;
            return NULL;
        }
        if (!found)
        {
            return result;
        }
    }
}

// ParamExpression

std::string ParamExpression::evaluate()
{
    if (getParams().hasParam(param))
        return getParams()[param];
    else
        return "[unknown parameter]";
}

bool CustomScript::ParamExpression::TryParse(ParseData & data, StringExpression *& expr)
{
    static const std::regex paramExp("<([^]+?)>");
    expr = NULL;
    std::smatch matches;
    if (!check_regex(data.bounds, matches, paramExp))
        return true;
    data.bounds.advance(matches[0].length());
    ParamExpression* typed = new ParamExpression(data.script);
    expr = typed;
    typed->param = matches[1];
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

bool Statement::parse(ParseData& data, ControlStatement* parent)
{
    ControlStatement* oldParent = data.parent;
    data.parent = parent;
    for (TryParseFunc func : TryParseList)
    {        
        if (func(data, next))
        {
            if (next)
            {
                if (data.bounds.begin == data.bounds.end)
                {
                    return true;
                }               
                bool result = next->parse(data, data.parent);
                data.parent = oldParent;
                return result;
            }
        }
        else
        {
            break;
        }
    }
    data.parent = oldParent;
    return false;
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

bool IfStatement::execute()
{
    bool success = true;
    if (evaluateCondition())
    {
        if (thenPart)
            success = thenPart->execute();
    }
    else
    {
        if (elsePart)
            success = elsePart->execute();
    }        
    return success && Statement::execute();
}

bool IfStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::regex ifExp("if");
    static const std::regex thenExp("then");
    static const std::regex elseExp("else");
    static const std::regex elseifExp("elseif");
    static const std::regex endExp("end");

    std::smatch matches;
    if (!check_regex(data.bounds, matches, ifExp))
        return true;

    data.bounds.advance(matches[0].length());

    BoolExpression* condition = BoolExpression::TryParse(data);
    if (!condition)
    {
        data.script->error("Boolean expression expected!");
        return false;
    }           

    if (!check_regex(data.bounds, matches, thenExp))
    {
        data.script->error("Then expected!");
        delete condition;
        return false;
    }
    data.bounds.advance(matches[0].length());

    IfStatement* typed = new IfStatement();
    stmt = typed;

    typed->setScript(data.script);
    typed->setParent(data.parent);  
    typed->condition = condition;
        
    Statement* thenPart = new Statement();
    typed->thenPart = thenPart;
    thenPart->parse(data, typed);

    IfStatement* lastElseif = typed;
    while (check_regex(data.bounds, matches, elseifExp))
    {
        data.bounds.advance(matches[0].length());
        IfStatement* then = new IfStatement();
        then->setScript(data.script);
        then->setParent(data.parent);
        then->condition = BoolExpression::TryParse(data);

        if (!then->condition)
        {
            data.script->error("Condition expected!");
            return false;
        }

        if (!check_regex(data.bounds, matches, thenExp))
        {
            data.script->error("Then expected!");
            delete condition;
            return false;
        }  
        data.bounds.advance(matches[0].length());

        lastElseif->elsePart = then;
        lastElseif = then;
        
        then->thenPart = new Statement();
        then->thenPart->parse(data, typed);
    }

    if (check_regex(data.bounds, matches, elseExp))
    {
        data.bounds.advance(matches[0].length());
        Statement* elsePart = new Statement();
        lastElseif->elsePart = elsePart;
        elsePart->parse(data, typed);
    }
    
    if (check_regex(data.bounds, matches, endExp))
    {
        data.bounds.advance(matches[0].length());
        return true;
    }

    data.script->error("End expected!");
    return false;
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

bool WhileStatement::TryParse(ParseData & data, Statement *& stmt)
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

bool DoWhileStatement::TryParse(ParseData & data, Statement *& stmt)
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

bool BreakStatement::TryParse(ParseData & data, Statement *& stmt)
{
    // TODO: BreakStatement::TryParse    
    return true;
}

// ContinueStatement

bool ContinueStatement::execute()
{
    return true;
}

bool ContinueStatement::TryParse(ParseData & data, Statement *& stmt)
{
    // TODO: ContinueStatement::TryParse
    return true;
}

// SkipStatement

bool SkipStatement::execute()
{
    return false;
}

bool SkipStatement::TryParse(ParseData & data, Statement *& stmt)
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

bool WriteStatement::execute()
{
    getAction()->write(stringExp->evaluate());
    return Statement::execute();
}

bool WriteStatement::TryParse(ParseData & data, Statement *& stmt)
{
    const static std::regex expWrite("write");
    
    std::smatch matches;
    if (!check_regex(data.bounds, matches, expWrite))
        return true;

    data.bounds.advance(matches[0].length());

    StringExpression* expr = StringConcatExpression::TryParse(data);
    if (!expr)
        return false;       

    WriteStatement* typed = new WriteStatement();
    stmt = typed;
    typed->stringExp = expr;
    typed->setParent(data.parent);
    typed->setScript(data.script);

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
    success = root.parse(parseData, NULL);
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

bool Script::succeeded() const
{
    return success;
}

void Script::error(std::string message) const
{
    size_t line = std::count(codeBegin, parseData.bounds.begin, '\n') + 1;
    size_t column = 42;
    ErrorDialog("CustomScript Error", 
                "Error in script " + title + 
                " at line " + std::to_string(line) + 
                " column " + std::to_string(column) +
                ":\n" + message);
}

// Global

bool CustomScript::check_regex(StringBounds bounds, std::smatch & matches, const std::regex & exp)
{
    return std::regex_search(bounds.begin, bounds.end, matches, exp, std::regex_constants::match_continuous);
}

void CustomScript::skipWhitespaces(StringBounds & bounds)
{
    static const std::regex whitespaces(ws0);
    std::smatch matches;
    if (check_regex(bounds, matches, whitespaces))
        bounds.advance(matches[0].length(), false);
}

// CustomAdventureAction

CustomAdventureAction::CustomAdventureAction(Adventure * adventure, std::string code, std::string title)
    : AdventureAction(adventure), script(this, code, title)
{
}

bool CustomAdventureAction::compileSucceeded() const
{
    return script.succeeded();
}

bool CustomAdventureAction::run(const Command::Result & params)
{
    return script.run(params);
}
