#include "stdafx.h"

#include "Adventure.h"
#include "AdventureAction.h"
#include "AdventureObject.h"
#include "Player.h"
#include "Room.h"
#include "Location.h"
#include "Item.h"
#include "RoomConnection.h"
#include "ItemCombiner.h"
#include "CustomAdventureAction.h" 
#include "CommandSystem.h"

#include "CustomScript.h"

using namespace CustomScript;

void StringBounds::advance(size_t amount, bool seekNext)
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
    skipLogicOp = false;
}

// Expression

Script * Expression::getScript() const
{
    return script;
}

const Command::Result & Expression::getParams() const
{
    return script->getParams();
}

CustomAdventureAction * Expression::getAction() const
{
    return script->getAction();
}

// ObjectExpression                     

const ObjectExpression::TryParseFunc ObjectExpression::TryParseList[] = {
    IdentExpression::TryParse
};

ObjectExpression * ObjectExpression::TryParse(ParseData & data)
{
    ObjectExpression* expr = NULL;
    for (TryParseFunc func : TryParseList)
    {
        auto oldBegin = data.bounds.begin;
        if (func(data, expr))
        {
            if (expr)
            {
                return expr;
            }
            data.bounds.begin = oldBegin;
        }
        else
        {
            break;
        }
    }
    return NULL;
}

ExpressionType ObjectExpression::getType()
{
    return etObject;
}

// StringExpression 

StringExpression * StringExpression::TryParse(ParseData & data)
{
    return StringConcatExpression::TryParse(data);
}

ExpressionType StringExpression::getType()
{
    return etString;
}

const StringExpression::TryParseFunc StringExpression::TryParseList[] = {
    ObjectToStringExpression::TryParse,
    ParamExpression::TryParse,
    ConstStringExpression::TryParse
};

// BoolExpression

const BoolExpression::TryParseFunc BoolExpression::TryParseList[] = {
    LogicOpExpression::TryParse,
    LogicNotExpression::TryParse,
    BracketExpression::TryParse,
    ParamIsIdentExpression::TryParse,
    ConstBoolExpression::TryParse,
    PlayerHasItemExpression::TryParse,
    LocationHasItemExpression::TryParse
};

BoolExpression * BoolExpression::TryParse(ParseData & data)
{
    BoolExpression* expr = NULL;
    for (TryParseFunc func : TryParseList)
    {
        if (data.skipLogicOp && func == LogicOpExpression::TryParse)
        {
            data.skipLogicOp = false;
            continue;
        }
        auto oldBegin = data.bounds.begin;
        if (func(data, expr))
        {
            if (expr)
            {
                return expr;
            }
            data.bounds.begin = oldBegin;
        }
        else
        {
            break;
        }
    }
    return NULL;
}

ExpressionType BoolExpression::getType()
{
    return etBool;
}

// IdentExpression

AdventureObject * IdentExpression::evaluate()
{
    return getAction()->getAdventure()->findObjectByName(identifier);
}

bool IdentExpression::TryParse(ParseData & data, ObjectExpression *& expr)
{
    static const std::regex identExp(":(" + ident + ")");

    std::smatch matches;
    if (!check_regex(data.bounds, matches, identExp))
        return true;
    data.bounds.advance(matches[0].length());
    IdentExpression* typed = new IdentExpression(data.script);
    expr = typed;
    typed->identifier = matches[1];
    return true;
}

// ObjectToStringExpression

ObjectToStringExpression::ObjectToStringExpression(Script * script)
    : StringExpression(script)
{
    objectExp = NULL;
}

ObjectToStringExpression::~ObjectToStringExpression()
{
    delete objectExp;
}

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

bool ObjectToStringExpression::TryParse(ParseData & data, StringExpression *& expr)
{
    static const std::regex typeExp("\\[([^]+?)\\]");

    ObjectToStringExpression* typed = new ObjectToStringExpression(data.script);

    typed->objectExp = ObjectExpression::TryParse(data);
    if (!typed->objectExp)
    {
        delete typed;
        return true;
    }

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
            delete typed;
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
            delete typed;
            return false;
        }
    }
    else
    {
        typed->startOfSentence = false;
        typed->type = gtArticleFromPlayer;
    }

    expr = typed;
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

    ConstStringExpression* typed = new ConstStringExpression(data.script);

    std::smatch matches;
    if (!check_regex(data.bounds, matches, stringExp))
    {
        delete typed;
        return true;
    }

    typed->text = std::regex_replace(matches[1].str(), escapeExp, "$1");
    data.bounds.advance(matches[0].length());

    expr = typed;
    return true;
}

// StringConcatExpression

StringConcatExpression::~StringConcatExpression()
{
    for (StringExpression* expr : stringExpList)
        delete expr;
}

std::string StringConcatExpression::evaluate()
{
    std::string result;
    for (StringExpression* exp : stringExpList)
        result += exp->evaluate();
    return result;
}

StringConcatExpression* StringConcatExpression::TryParse(ParseData & data)
{
    StringConcatExpression* result = new StringConcatExpression(data.script);
    StringExpression* sub;
    bool foundFirst = false;
    while (true)
    {
        bool found = false;
        sub = NULL;
        for (TryParseFunc func : TryParseList)
        {
            auto oldBegin = data.bounds.begin;
            if (func(data, sub))
            {
                if (sub)
                {
                    result->stringExpList.push_back(sub);
                    found = true;
                    foundFirst = true;
                    break;
                }
                data.bounds.begin = oldBegin;
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

bool ParamExpression::TryParse(ParseData & data, StringExpression *& expr)
{
    static const std::regex paramExp("<([^]+?)>");

    ParamExpression* typed = new ParamExpression(data.script);

    std::smatch matches;
    if (!check_regex(data.bounds, matches, paramExp))
    {
        delete typed;
        return true;
    }
    data.bounds.advance(matches[0].length());
    typed->param = matches[1];
    data.script->getRequiredParams().insert(typed->param);

    expr = typed;
    return true;
}

// IdentAsStringExpression

std::string IdentAsStringExpression::evaluate()
{
    return identString;
}

IdentAsStringExpression* IdentAsStringExpression::TryParse(ParseData & data)
{
    static const std::regex identExp(ident);
    std::smatch matches;
    if (!check_regex(data.bounds, matches, identExp))
        return NULL;
    data.bounds.advance(matches[0].length());

    IdentAsStringExpression* typed = new IdentAsStringExpression(data.script);
    typed->identString = matches[0];
    return typed;
}

// ParamIsIdentExpression

ParamIsIdentExpression::ParamIsIdentExpression(Script * script)
    : BoolExpression(script)
{
    paramExp = NULL;
    identExp = NULL;
}

ParamIsIdentExpression::~ParamIsIdentExpression()
{
    delete paramExp;
    delete identExp;
}

bool ParamIsIdentExpression::evaluate()
{
    return getAction()->getAdventure()->findObjectByAlias(paramExp->evaluate()) == identExp->evaluate();
}

bool ParamIsIdentExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex isExp("is");

    ParamIsIdentExpression* typed = new ParamIsIdentExpression(data.script);

    StringExpression* paramExp = NULL;
    ObjectExpression* identExp = NULL;

    if (!ParamExpression::TryParse(data, paramExp))
    {
        delete typed;
        return false;
    }
    if (!paramExp)
    {
        delete typed;
        return true;
    }
    typed->paramExp = (ParamExpression*)paramExp;

    std::smatch matches;
    if (!check_regex(data.bounds, matches, isExp))
        return true;
    data.bounds.advance(matches[0].length());

    if (!IdentExpression::TryParse(data, identExp))
    {
        delete typed;
        return false;
    }
    if (!identExp)
    {
        delete typed;
        return true;
    }
    typed->identExp = (IdentExpression*)identExp;

    expr = typed;
    return true;
}

// ConstBoolExpression

bool ConstBoolExpression::evaluate()
{
    return value;
}

bool ConstBoolExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex boolExp("(true|false)");

    std::smatch matches;
    if (!check_regex(data.bounds, matches, boolExp))
        return true;
    data.bounds.advance(matches[0].length());
    ConstBoolExpression* typed = new ConstBoolExpression(data.script);
    expr = typed;
    typed->value = matches[1] == "true";
    return true;
}

// PlayerHasItemExpression

PlayerHasItemExpression::PlayerHasItemExpression(Script * script)
    : BoolExpression(script)
{
    itemExp = NULL;
}

PlayerHasItemExpression::~PlayerHasItemExpression()
{
    delete itemExp;
}

bool PlayerHasItemExpression::evaluate()
{
    Item* item = dynamic_cast<Item*>(itemExp->evaluate());
    if (!item)
    {
        ErrorDialog("Not a valid item!");
        return false;
    }
    return getAction()->getPlayerInv()->hasItem(item);
}

bool PlayerHasItemExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex playerHasExp("player has");

    PlayerHasItemExpression* typed = new PlayerHasItemExpression(data.script);

    std::smatch matches;
    if (!check_regex(data.bounds, matches, playerHasExp))
    {
        delete typed;
        return true;
    }
    data.bounds.advance(matches[0].length());

    typed->itemExp = ObjectExpression::TryParse(data);
    if (!typed->itemExp)
    {
        delete typed;
        return false;
    }

    expr = typed;
    return true;
}

// BracketExpression

BracketExpression::BracketExpression(Script * script)
    : BoolExpression(script)
{
    boolExp = NULL;
}

BracketExpression::~BracketExpression()
{
    delete boolExp;
}

bool BracketExpression::evaluate()
{
    return boolExp->evaluate();
}

bool BracketExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex openingBracket("\\(");
    static const std::regex closingBracket("\\)");
    std::smatch matches;
    if (!check_regex(data.bounds, matches, openingBracket))
        return true;
    data.bounds.advance(matches[0].length());
    expr = BoolExpression::TryParse(data);
    if (!expr)
        return false;
    if (!check_regex(data.bounds, matches, closingBracket))
    {
        data.script->error("Closing bracket \")\" expected!");
        delete expr;
        return false;
    }
    data.bounds.advance(matches[0].length());

    return true;
}

// LogicNotExpression

CustomScript::LogicNotExpression::LogicNotExpression(Script * script)
    : BoolExpression(script)
{
    boolExp = NULL;
}

CustomScript::LogicNotExpression::~LogicNotExpression()
{
    delete boolExp;
}

bool CustomScript::LogicNotExpression::evaluate()
{
    return !boolExp->evaluate();
}

bool CustomScript::LogicNotExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex notExp("not");

    LogicNotExpression* typed = new LogicNotExpression(data.script);

    std::smatch matches;
    if (!check_regex(data.bounds, matches, notExp))
    {
        delete typed;
        return true;
    }
    data.bounds.advance(matches[0].length());

    typed->boolExp = BoolExpression::TryParse(data);
    if (!typed->boolExp)
    {
        delete typed;
        return false;
    }

    expr = typed;
    return true;
}

// LogicOpExpression

LogicOpExpression::LogicOpExpression(Script * script)
    : BoolExpression(script)
{
    boolExp1 = NULL;
    boolExp2 = NULL;
}

LogicOpExpression::~LogicOpExpression()
{
    delete boolExp1;
    delete boolExp2;
}

bool LogicOpExpression::evaluate()
{
    switch (operation)
    {
    case opAND:
        return boolExp1->evaluate() && boolExp2->evaluate();
    case opOR:
        return boolExp1->evaluate() || boolExp2->evaluate();
    case opXOR:
        return boolExp1->evaluate() != boolExp2->evaluate();
    }
    ErrorDialog("Unknown logical operation!");
    return false;
}

bool LogicOpExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::regex operatorExp("(and|or|xor)");

    LogicOpExpression* typed = new LogicOpExpression(data.script);

    data.skipLogicOp = true;
    typed->boolExp1 = BoolExpression::TryParse(data);
    if (!typed->boolExp1)
    {
        delete typed;
        return true;
    }

    std::smatch matches;
    if (!check_regex(data.bounds, matches, operatorExp))
    {
        delete typed;
        return true;
    }
    data.bounds.advance(matches[0].length());

    typed->boolExp2 = BoolExpression::TryParse(data);
    if (!typed->boolExp2)
    {
        delete typed;
        return false;
    }

    if (matches[0] == "and")
        typed->operation = opAND;
    else if (matches[0] == "or")
        typed->operation = opOR;
    else
        typed->operation = opXOR;

    LogicOpExpression* current = typed;
    bool repeat;
    do
    {
        repeat = false;
        if (LogicOpExpression* next = dynamic_cast<LogicOpExpression*>(current->boolExp2))
        {
            if (!(current->operation != opAND && next->operation == opAND))
            {
                // a and (b or c)
                current->boolExp2 = next->boolExp2;
                next->boolExp2 = next->boolExp1;
                next->boolExp1 = current->boolExp1;
                current->boolExp1 = next;
                // (a or b) and c
                LogicalOperation tmp = next->operation;
                next->operation = current->operation;
                current->operation = tmp;
                // (a and b) or c
                current = next;
                repeat = true;
            }
        }
    }
    while (repeat);

    expr = typed;
    return true;
}

// LocationHasItemExpression

LocationHasItemExpression::LocationHasItemExpression(Script * script)
    : BoolExpression(script)
{
    locationExp = NULL;
    itemExp = NULL;
    prepositionExp = NULL;
}

LocationHasItemExpression::~LocationHasItemExpression()
{
    delete locationExp;
    delete itemExp;
    delete prepositionExp;
}

bool LocationHasItemExpression::evaluate()
{
    Location* location = dynamic_cast<Location*>(locationExp->evaluate());
    if (!location)
    {
        ErrorDialog("Not a valid location!");
        return false;
    }
    Item* item = dynamic_cast<Item*>(itemExp->evaluate());
    if (!item)
    {
        ErrorDialog("Not a valid item!");
        return false;
    }

    if (Location::PInventory* inv = location->getInventory(prepositionExp->evaluate()))
    {
        return inv->hasItem(item);
    }
    else
    {
        ErrorDialog("Not a valid preposition!");
        return false;
    }
}

bool LocationHasItemExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    LocationHasItemExpression* typed = new LocationHasItemExpression(data.script);

    typed->itemExp = ObjectExpression::TryParse(data);
    if (!typed->itemExp)
    {
        delete typed;
        return true;
    }

    typed->prepositionExp = IdentAsStringExpression::TryParse(data);
    if (!typed->prepositionExp)
    {
        delete typed;
        return false;
    }

    typed->locationExp = ObjectExpression::TryParse(data);
    if (!typed->locationExp)
    {
        delete typed;
        return false;
    }

    expr = typed;
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
    SwitchStatement::TryParse,
    ProcedureStatement::TryParse
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

Statement::ParseResult Statement::parse(ParseData& data, ControlStatement* parent)
{
    ControlStatement* oldParent = data.parent;
    data.parent = parent;
    ParseResult result = prUnknownCommand;
    for (TryParseFunc func : TryParseList)
    {
        if (func(data, next))
        {
            if (next)
            {
                if (data.bounds.begin == data.bounds.end)
                {
                    return prSuccess;
                }
                result = next->parse(data, data.parent);
                break;
            }
        }
        else
        {
            result = prError;
            break;
        }
    }
    data.parent = oldParent;
    return result;
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

// SwitchStatement

SwitchStatement::CaseSection::CaseSection(IdentExpression * ident, Statement * statement)
{
    this->ident = ident;
    this->statement = statement;
}

SwitchStatement::SwitchStatement()
{
    switchPart = NULL;
    elsePart = NULL;
}

SwitchStatement::~SwitchStatement()
{
    delete switchPart;
    delete elsePart;
    for (CaseSection section : caseParts)
    {
        delete section.ident;
        delete section.statement;
    }
}

bool SwitchStatement::execute()
{
    AdventureObject* object = getAction()->getAdventure()->findObjectByAlias(switchPart->evaluate());
    bool success = true;
    bool found = false;
    if (object)
    {
        for (CaseSection section : caseParts)
        {
            if (object == section.ident->evaluate())
            {
                success = section.statement->execute();
                found = true;
                break;
            }
        }
    }
    if (!found && elsePart)
        success = elsePart->execute();
    return success && ControlStatement::execute();
}

bool SwitchStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::regex caseExp("case");
    static const std::regex ofExp("of");
    static const std::regex labelExp(":");
    static const std::regex elseExp("else");
    static const std::regex endExp("end");

    std::smatch matches;
    if (!check_regex(data.bounds, matches, caseExp))
        return true;
    data.bounds.advance(matches[0].length());

    StringExpression* switchPart = NULL;
    if (!ParamExpression::TryParse(data, switchPart))
        return false;
    if (!switchPart)
    {
        data.script->error("Paremeter expression expected");
        return false;
    }

    if (!check_regex(data.bounds, matches, ofExp))
    {
        data.script->error("Of expected");
        return false;
    }
    data.bounds.advance(matches[0].length());

    SwitchStatement* typed = new SwitchStatement();
    typed->switchPart = (ParamExpression*)switchPart;
    typed->setScript(data.script);
    typed->setParent(data.parent);

    while (check_regex(data.bounds, matches, labelExp))
    {
        data.bounds.advance(matches[0].length());
        ObjectExpression* expr = NULL;
        if (!IdentExpression::TryParse(data, expr))
            return false;
        if (!expr)
        {
            data.script->error("Ident expression expected");
            return false;
        }

        IdentExpression* iexpr = (IdentExpression*)expr;

        Statement* caseStmt = new Statement();
        typed->caseParts.push_back(CaseSection((IdentExpression*)expr, caseStmt));
        if (caseStmt->parse(data, typed) == prError)
            return false;
    }

    if (check_regex(data.bounds, matches, elseExp))
    {
        data.bounds.advance(matches[0].length());
        Statement* elseStatement = new Statement();
        typed->elsePart = elseStatement;
        if (elseStatement->parse(data, typed) == prError)
            return false;
    }

    if (!check_regex(data.bounds, matches, endExp))
    {
        data.script->error("End expected");
        return false;
    }
    data.bounds.advance(matches[0].length());

    stmt = typed;
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

ProcedureStatement::ProcedureData::ProcedureData(std::string name, std::vector<ExpressionType> params)
{
    this->name = name;
    this->params = params;
}

const ProcedureStatement::ProcedureData ProcedureStatement::Functions[PROCEDURE_COUNT] = {
    ProcedureData("write",{ etString }), // write text
    ProcedureData("draw",{ etString }), // draw filename

    ProcedureData("set_room",{ etObject }), // set_room room
    ProcedureData("set_location",{ etObject }), // set_location location

    ProcedureData("player_add_item",{ etObject }),              // player_add_item item
    ProcedureData("player_del_item",{ etObject }),              // player_del_item item
    ProcedureData("location_add_item",{ etObject, etIdent, etObject }), // location_add_item item, preposition, location
    ProcedureData("location_del_item",{ etObject, etIdent, etObject }), // location_del_item item, preposition, location

    ProcedureData("filter_add",{ etObject, etIdent, etObject }), // filter_add item, preposition, location
    ProcedureData("filter_del",{ etObject, etIdent, etObject }), // filter_del item, preposition, location
    ProcedureData("filter_whitelist",{ etIdent, etObject }),           // filter_whitelist preposition, location
    ProcedureData("filter_blacklist",{ etIdent, etObject }),           // filter_blacklist preposition, location
    ProcedureData("filter_disable",{ etIdent, etObject }),           // filter_disable preposition, location

    ProcedureData("set_description",{ etObject, etString }), // set_description object, description
    ProcedureData("add_alias",{ etObject, etString }), // add_alias object, alias
    ProcedureData("del_alias",{ etObject, etString }), // del_alias object, alias

    ProcedureData("player_inform",{ etObject }), // player_inform object
    ProcedureData("player_forget",{ etObject }), // player_forget object    

    ProcedureData("lock",{ etObject }), // lock room_connection
    ProcedureData("unlock",{ etObject }), // unlock room_connection

    ProcedureData("add_item_combination",{ etObject, etObject, etObject }), // add_item_combination item1, item2, item_out
    ProcedureData("del_item_combination",{ etObject, etObject }),           // del_item_combination item1, item2 
};

ProcedureStatement::ProcedureStatement()
{
}

ProcedureStatement::~ProcedureStatement()
{
    for (Expression* expr : params)
        delete expr;
}

bool ProcedureStatement::execute()
{
    /* --- Templates ---
    std::string _ = ((StringExpression*)params[i])->evaluate();
    bool _ = ((BoolExpression*)params[i])->evaluate();
    ObjectType* _ = dynamic_cast<ObjectType*>(((ObjectExpression*)params[i])->evaluate());

    if (!_)
    ErrorDialog("Not a valid ObjectType!");
    else
    // Just do it!
    */
    switch (type)
    {
    case ptWrite: {
        std::string text = ((StringExpression*)params[0])->evaluate();
        getAction()->write(text);
        break;
    }
    case ptDraw: {
        ErrorDialog("Draw not yet implemented!");
        break;
    }

    case ptSetRoom: {
        Room* room = dynamic_cast<Room*>(((ObjectExpression*)params[0])->evaluate());
        if (!room)
            ErrorDialog("Not a valid room!");
        else
            getAction()->getPlayer()->gotoRoom(room);
        break;
    }
    case ptSetLocation: {
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[0])->evaluate());
        if (!location)
            ErrorDialog("Not a valid location!");
        else
        {
            getAction()->getPlayer()->gotoRoom(location->getRoom());
            getAction()->getPlayer()->gotoLocation(location);
        }
        break;
    }

    case ptPlayerAddItem: {
        Item* item = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        if (!item)
            ErrorDialog("Not a valid item!");
        else
            getAction()->getPlayerInv()->addItem(item);
        break;
    }
    case ptPlayerDelItem: {
        Item* item = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        if (!item)
            ErrorDialog("Not a valid item!");
        else if (!getAction()->getPlayerInv()->delItem(item))
            ErrorDialog("Played did not have item!");
        break;
    }
    case ptLocationAddItem: {
        Item* item = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        std::string preposition = ((StringExpression*)params[1])->evaluate();
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[2])->evaluate());

        if (!location)
            ErrorDialog("Not a valid location!");
        else if (!item)
            ErrorDialog("Not a valid item!");
        else if (Location::PInventory* inv = location->getInventory(preposition))
            inv->addItemForce(item);
        else
            ErrorDialog("Not a valid preposition!");
        break;
    }
    case ptLocationDelItem: {
        Item* item = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        std::string preposition = ((StringExpression*)params[1])->evaluate();
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[2])->evaluate());

        if (!location)
            ErrorDialog("Not a valid location!");
        else if (!item)
            ErrorDialog("Not a valid item!");
        else if (Location::PInventory* inv = location->getInventory(preposition))
        {
            if (!inv->delItem(item))
                ErrorDialog("Location did not have item!");
        }
        else
            ErrorDialog("Not a valid preposition!");
        break;
    }

    case ptFilterAdd: {
        Item* item = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        std::string preposition = ((StringExpression*)params[1])->evaluate();
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[2])->evaluate());

        if (!location)
            ErrorDialog("Not a valid location!");
        else if (!item)
            ErrorDialog("Not a valid item!");
        else if (Location::PInventory* inv = location->getInventory(preposition))
            inv->addToFilter(item);
        else
            ErrorDialog("Not a valid preposition!");
        break;
    }
    case ptFilterDel: {
        Item* item = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        std::string preposition = ((StringExpression*)params[1])->evaluate();
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[2])->evaluate());

        if (!location)
            ErrorDialog("Not a valid location!");
        else if (!item)
            ErrorDialog("Not a valid item!");
        else if (Location::PInventory* inv = location->getInventory(preposition))
        {
            if (!inv->delFromFilter(item))
                ErrorDialog("Filter did not have item!");
        }
        else
            ErrorDialog("Not a valid preposition!");
        break;
    }
    case ptFilterWhitelist: {
        std::string preposition = ((StringExpression*)params[0])->evaluate();
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[1])->evaluate());

        if (!location)
            ErrorDialog("Not a valid location!");
        else if (Location::PInventory* inv = location->getInventory(preposition))
            inv->enableFilter(Location::PInventory::ifWhitelist);
        else
            ErrorDialog("Not a valid preposition!");
        break;
    }
    case ptFilterBlacklist: {
        std::string preposition = ((StringExpression*)params[0])->evaluate();
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[1])->evaluate());

        if (!location)
            ErrorDialog("Not a valid location!");
        else if (Location::PInventory* inv = location->getInventory(preposition))
            inv->enableFilter(Location::PInventory::ifBlacklist);
        else
            ErrorDialog("Not a valid preposition!");
        break;
    }
    case ptFilterDisable: {
        std::string preposition = ((StringExpression*)params[0])->evaluate();
        Location* location = dynamic_cast<Location*>(((ObjectExpression*)params[1])->evaluate());

        if (!location)
            ErrorDialog("Not a valid location!");
        else if (Location::PInventory* inv = location->getInventory(preposition))
            inv->disableFilter();
        else
            ErrorDialog("Not a valid preposition!");
        break;
    }

    case ptSetDescription: {
        AdventureObject* object = dynamic_cast<AdventureObject*>(((ObjectExpression*)params[0])->evaluate());
        std::string description = ((StringExpression*)params[1])->evaluate();

        if (!object)
            ErrorDialog("Not a valid object!");
        else
            object->setDescription(description);
        break;
    }
    case ptAddAlias: {
        AdventureObject* object = dynamic_cast<AdventureObject*>(((ObjectExpression*)params[0])->evaluate());
        std::string alias = ((StringExpression*)params[1])->evaluate();

        if (!object)
            ErrorDialog("Not a valid object!");
        else
            object->getAliases().add(alias);
        break;
    }
    case ptDelAlias: {
        AdventureObject* object = dynamic_cast<AdventureObject*>(((ObjectExpression*)params[0])->evaluate());
        std::string alias = ((StringExpression*)params[1])->evaluate();

        if (!object)
            ErrorDialog("Not a valid object!");
        else if (!object->getAliases().del(alias))
            ErrorDialog("Object did not have specified alias!");
        break;
    }

    case ptPlayerInform: {
        AdventureObject* object = dynamic_cast<AdventureObject*>(((ObjectExpression*)params[0])->evaluate());

        if (!object)
            ErrorDialog("Not a valid object");
        else
            getAction()->getPlayer()->inform(object);
        break;
    }
    case ptPlayerForget: {
        AdventureObject* object = dynamic_cast<AdventureObject*>(((ObjectExpression*)params[0])->evaluate());

        if (!object)
            ErrorDialog("Not a valid object");
        else
            getAction()->getPlayer()->forget(object);
        break;
    }

    case ptLock: {
        RoomConnection* connection = dynamic_cast<RoomConnection*>(((ObjectExpression*)params[0])->evaluate());

        if (!connection)
            ErrorDialog("Not a valid room connection!");
        else
            connection->lock();
        break;
    }
    case ptUnlock: {
        RoomConnection* connection = dynamic_cast<RoomConnection*>(((ObjectExpression*)params[0])->evaluate());

        if (!connection)
            ErrorDialog("Not a valid room connection!");
        else
            connection->unlock();
        break;
    }

    case ptAddItemCombination: {
        Item* item1 = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        Item* item2 = dynamic_cast<Item*>(((ObjectExpression*)params[1])->evaluate());
        Item* item_out = dynamic_cast<Item*>(((ObjectExpression*)params[2])->evaluate());

        if (!item1 || !item2 || !item_out)
            ErrorDialog("Not a valid item!");
        else if (!getAction()->getItemCombiner()->addCombination(item1, item2, item_out))
            ErrorDialog("Item combination exists already!");
        break;
    }
    case ptDelItemCombination: {
        Item* item1 = dynamic_cast<Item*>(((ObjectExpression*)params[0])->evaluate());
        Item* item2 = dynamic_cast<Item*>(((ObjectExpression*)params[1])->evaluate());

        if (!item1 || !item2)
            ErrorDialog("Not a valid item!");
        else if (!getAction()->getItemCombiner()->delCombination(item1, item2))
            ErrorDialog("Item combination does not exist!");
        break;
    }
    }
    return Statement::execute();
}

bool ProcedureStatement::TryParse(ParseData & data, Statement *& stmt)
{
    const static std::regex identExp(ident);

    std::smatch matches;
    if (!check_regex(data.bounds, matches, identExp))
        return true;

    bool found = false;
    ProcedureType funcType;
    for (funcType = ptWrite; funcType < PROCEDURE_COUNT; funcType = (ProcedureType)(funcType + 1))
    {
        if (Functions[funcType].name == matches[0])
        {
            found = true;
            break;
        }
    }
    if (!found)
        return true; // function does not exist

    ProcedureStatement* typed = new ProcedureStatement();
    stmt = typed;
    typed->setParent(data.parent);
    typed->setScript(data.script);
    typed->type = funcType;

    data.bounds.advance(matches[0].length());

    for (ExpressionType exprType : Functions[typed->type].params)
    {
        Expression* expr = NULL;
        switch (exprType)
        {
        case etObject:
            expr = ObjectExpression::TryParse(data);
            break;
        case etBool:
            expr = BoolExpression::TryParse(data);
            break;
        case etString:
            expr = StringExpression::TryParse(data);
            break;
        case etIdent:
            expr = IdentAsStringExpression::TryParse(data);
            break;
        }

        if (!expr)
            return false;
        typed->params.push_back(expr);
    }

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
    if (codeBegin == parseData.bounds.end)
        success = true;
    else
        switch (root.parse(parseData, NULL))
        {
        case Statement::prSuccess:
            success = true;
            break;
        case Statement::prUnknownCommand:
            error("Unknown command at end");
        case Statement::prError:
            success = false;
            break;
        }
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

tags & CustomScript::Script::getRequiredParams()
{
    return requiredParams;
}

bool Script::succeeded() const
{
    return success;
}

void Script::error(std::string message) const
{
    size_t line = std::count(codeBegin, parseData.bounds.begin, '\n') + 1;
    int column;
    for (column = 0; column < parseData.bounds.begin - codeBegin; column++)
        if (*(parseData.bounds.begin - column) == '\n')
            break;

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
