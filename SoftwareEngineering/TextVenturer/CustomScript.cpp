#include "stdafx.h"

#include "AliasList.h"
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
    pos += amount;
    if (seekNext)
        skipWhitespaces(*this);
}

CustomScript::StringBounds::StringBounds(const std::string & text, size_t pos, size_t end)
    : text(text)
    , pos(pos)
    , end(end)
{
    if (end == std::string::npos)
        this->end = text.size() - pos;
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

Expression * CustomScript::Expression::loadTyped(FileStream & stream, ExpressionType type, Script * script)
{
    switch (type)
    {                  
    case etObject:
        return ObjectExpression::loadTyped(stream, script);
    case etString:
    case etIdent:
        return StringExpression::loadTyped(stream, script);
    case etBool:
        return BoolExpression::loadTyped(stream, script);
    }
    return NULL;
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
        size_t oldPos = data.bounds.pos;
        if (func(data, expr))
        {
            if (expr)
            {
                return expr;
            }
            data.bounds.pos = oldPos;
        }
        else
        {
            break;
        }
    }
    return NULL;
}

ExpressionType ObjectExpression::getResultType()
{
    return etObject;
}

ObjectExpression * CustomScript::ObjectExpression::loadTyped(FileStream & stream, Script * script)
{
    ObjectExpression* expr = NULL;
    switch (static_cast<Type>(stream.readByte()))
    {
    case etIdent:
        expr = new IdentExpression(script);
        break;            
    }
    expr->load(stream, script);
    return expr;
}

void CustomScript::ObjectExpression::save(FileStream & stream)
{
    stream.write(static_cast<byte>(getType()));
}

// StringExpression 

StringExpression * StringExpression::TryParse(ParseData & data)
{
    return StringConcatExpression::TryParse(data);
}

ExpressionType StringExpression::getResultType()
{
    return etString;
}

StringExpression * CustomScript::StringExpression::loadTyped(FileStream & stream, Script * script)
{
    StringExpression* expr = NULL;
    switch (static_cast<Type>(stream.readByte()))
    {
    case etObjectToString:
        expr = new ObjectToStringExpression(script);
        break;
    case etConstString:
        expr = new ConstStringExpression(script);
        break;
    case etStringConcat:
        expr = new StringConcatExpression(script);
        break;
    case etParam:
        expr = new ParamExpression(script);
        break;
    case etIdentAsString:
        expr = new IdentAsStringExpression(script);
        break;
    }
    expr->load(stream, script);
    return expr;
}

void CustomScript::StringExpression::save(FileStream & stream)
{
    stream.write(static_cast<byte>(getType()));
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
    LocationHasItemExpression::TryParse,
    ObjectFlagTestExpression::TryParse,
    GlobalFlagTestExpression::TryParse
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
        size_t oldPos = data.bounds.pos;
        if (func(data, expr))
        {
            if (expr)
            {
                return expr;
            }
            data.bounds.pos = oldPos;
        }
        else
        {
            break;
        }
    }
    return NULL;
}

ExpressionType BoolExpression::getResultType()
{
    return etBool;
}

BoolExpression * CustomScript::BoolExpression::loadTyped(FileStream & stream, Script * script)
{
    BoolExpression* expr = NULL;
    switch (static_cast<Type>(stream.readByte()))
    {
    case etParamIsIdent:
        expr = new ParamIsIdentExpression(script);
        break;
    case etConstBool:
        expr = new ConstBoolExpression(script);
        break;
    case etPlayerHasItem:
        expr = new PlayerHasItemExpression(script);
        break;
    case etBracket:
        expr = new BracketExpression(script);
        break;
    case etLogicNot:
        expr = new LogicNotExpression(script);
        break;
    case etLogicOp:
        expr = new LogicOpExpression(script);
        break;
    case etLocationHasItem:
        expr = new LocationHasItemExpression(script);
        break;
    case etObjectFlagTest:
        expr = new ObjectFlagTestExpression(script);
        break;
    case etGlobalFlagTest:
        expr = new GlobalFlagTestExpression(script);
        break;
    }
    expr->load(stream, script);
    return expr;
}

void CustomScript::BoolExpression::save(FileStream & stream)
{
    stream.write(static_cast<byte>(getType()));
}

// IdentExpression

ObjectExpression::Type CustomScript::IdentExpression::getType()
{
    return etIdent;
}

AdventureObject * IdentExpression::evaluate()
{
    try
    {
        return getAction()->getAdventure()->findObjectByName(identifier);
    }
    catch (EAdventureObjectNameNotFound)
    {
        throw(EObjectEvaluation, identifier);
    }
}

bool IdentExpression::TryParse(ParseData & data, ObjectExpression *& expr)
{
    static const std::string identPrefixExp(":");

    if (!quick_check(data.bounds, identPrefixExp))
        return true;

    data.bounds.pos += identPrefixExp.size();

    std::string ident;
    if (!parse_ident(data.bounds, ident))
        return true;

    data.bounds.advance(ident.size());

    IdentExpression* typed = new IdentExpression(data.script);
    expr = typed;
    typed->identifier = ident;
    return true;
}

void CustomScript::IdentExpression::save(FileStream & stream)
{
    ObjectExpression::save(stream);
    stream.write(identifier);
}

void CustomScript::IdentExpression::load(FileStream & stream, Script * script)
{
    stream.read(identifier);
}

// ObjectToStringExpression

StringExpression::Type CustomScript::ObjectToStringExpression::getType()
{
    return etObjectToString;
}

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
    static const std::string openingBracketExp("[");
    static const std::string closingBracketExp("]");

    ObjectToStringExpression* typed = new ObjectToStringExpression(data.script);

    typed->objectExp = ObjectExpression::TryParse(data);
    if (!typed->objectExp)
    {
        delete typed;
        return true;
    }

    std::smatch matches;
    if (quick_check(data.bounds, openingBracketExp))
    {
        data.bounds.advance(openingBracketExp.size());

        if (data.bounds.pos == data.bounds.end)
        {
            data.script->error("Expected p/d/i/n but got end.");
            return false;
        }

        char c = data.bounds.text[data.bounds.pos];
        data.bounds.advance(1);

        static const std::string typeHelp =
            "\n  Valid chars are:"
            "\n    [p] Definite article if player knows object (default)"
            "\n    [d] Force definite article"
            "\n    [i] Force indefinite article"
            "\n    [n] name only"
            "\n  char is uppercase -> first char of name captial";

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

        if (!quick_check(data.bounds, closingBracketExp))
        {
            data.script->error("Expected ] to enclose Identifier-Type.");
            return false;
        }
        data.bounds.advance(1);
    }
    else
    {
        typed->startOfSentence = false;
        typed->type = gtArticleFromPlayer;
    }

    expr = typed;
    return true;
}

void CustomScript::ObjectToStringExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    objectExp->save(stream);
    stream.write(startOfSentence);
    stream.write(static_cast<byte>(type));
}

void CustomScript::ObjectToStringExpression::load(FileStream & stream, Script * script)
{
    objectExp = ObjectExpression::loadTyped(stream, script);
    stream.read(startOfSentence);
    type = static_cast<GenerateType>(stream.readByte());
}

// ConstStringExpression

StringExpression::Type CustomScript::ConstStringExpression::getType()
{
    return etConstString;
}

std::string ConstStringExpression::evaluate()
{
    return text;
}

bool ConstStringExpression::TryParse(ParseData & data, StringExpression *& expr)
{
    ConstStringExpression* typed = new ConstStringExpression(data.script);

    std::smatch matches;
    std::string text;
    if (!parse_string(data.bounds, typed->text))
    {
        delete typed;
        return true;
    }

    expr = typed;
    return true;
}

void CustomScript::ConstStringExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(text);
}

void CustomScript::ConstStringExpression::load(FileStream & stream, Script * script)
{
    stream.read(text);
}

// StringConcatExpression

StringExpression::Type CustomScript::StringConcatExpression::getType()
{
    return etStringConcat;
}

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
            size_t oldPos = data.bounds.pos;
            if (func(data, sub))
            {
                if (sub)
                {
                    result->stringExpList.push_back(sub);
                    found = true;
                    foundFirst = true;
                    break;
                }
                data.bounds.pos = oldPos;
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

void CustomScript::StringConcatExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(static_cast<UINT>(stringExpList.size()));
    for (StringExpression* stringExpr : stringExpList)
        stringExpr->save(stream);
}

void CustomScript::StringConcatExpression::load(FileStream & stream, Script * script)
{
    UINT length = stream.readUInt(); 
    for (UINT i = 0; i < length; i++)
        stringExpList.push_back(StringExpression::loadTyped(stream, script));
}

// ParamExpression

StringExpression::Type CustomScript::ParamExpression::getType()
{
    return etParam;
}

std::string ParamExpression::evaluate()
{
    if (!getParams().hasParam(param))
        throw(EUnknownParam, param);
    return getParams()[param];
}

bool ParamExpression::TryParse(ParseData & data, StringExpression *& expr)
{
    static const std::string openingBracketExp("<");

    ParamExpression* typed = new ParamExpression(data.script);

    if (!quick_check(data.bounds, "<"))
    {
        delete typed;
        return true;
    }

    data.bounds.pos += openingBracketExp.size();

    size_t end = data.bounds.text.find('>', data.bounds.pos);
    if (end == std::string::npos)
    {
        data.script->error("Expected > closing bracket.");
        return false;
    }

    typed->param = data.bounds.text.substr(data.bounds.pos, end - data.bounds.pos);
    data.script->getRequiredParams().insert(typed->param);

    data.bounds.advance(end - data.bounds.pos + 1);

    expr = typed;
    return true;
}

void CustomScript::ParamExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(param);
}

void CustomScript::ParamExpression::load(FileStream & stream, Script * script)
{
    stream.read(param);
}

// IdentAsStringExpression

StringExpression::Type CustomScript::IdentAsStringExpression::getType()
{
    return etIdentAsString;
}

std::string IdentAsStringExpression::evaluate()
{
    return identString;
}

IdentAsStringExpression* IdentAsStringExpression::TryParse(ParseData & data)
{
    std::string ident;
    if (!parse_ident(data.bounds, ident))
        return NULL;

    data.bounds.advance(ident.length());

    IdentAsStringExpression* typed = new IdentAsStringExpression(data.script);
    typed->identString = ident;
    return typed;
}

void CustomScript::IdentAsStringExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(identString);
}

void CustomScript::IdentAsStringExpression::load(FileStream & stream, Script * script)
{
    stream.read(identString);
}

// ParamIsIdentExpression

BoolExpression::Type CustomScript::ParamIsIdentExpression::getType()
{
    return etParamIsIdent;
}

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
    static const std::string isExp("is");

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

    if (!quick_check(data.bounds, isExp))
        return true;
    data.bounds.advance(isExp.size());

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

void CustomScript::ParamIsIdentExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    paramExp->save(stream);
    identExp->save(stream);
}

void CustomScript::ParamIsIdentExpression::load(FileStream & stream, Script * script)
{
    paramExp = static_cast<ParamExpression*>(StringExpression::loadTyped(stream, script));
    identExp = static_cast<IdentExpression*>(ObjectExpression::loadTyped(stream, script));
}

// ConstBoolExpression

BoolExpression::Type CustomScript::ConstBoolExpression::getType()
{
    return etConstBool;
}

bool ConstBoolExpression::evaluate()
{
    return value;
}

bool ConstBoolExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::string boolTrueExp("true");
    static const std::string boolFalseExp("false");
    
    std::smatch matches;
    if (quick_check(data.bounds, boolTrueExp))
    {
        data.bounds.advance(boolTrueExp.size());
        ConstBoolExpression* typed = new ConstBoolExpression(data.script);
        expr = typed;
        typed->value = true;
    }
    else if (quick_check(data.bounds, boolFalseExp))
    {
        data.bounds.advance(boolFalseExp.size());
        ConstBoolExpression* typed = new ConstBoolExpression(data.script);
        expr = typed;
        typed->value = false;
    }   
    return true;
}

void CustomScript::ConstBoolExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    stream.write(value);
}

void CustomScript::ConstBoolExpression::load(FileStream & stream, Script * script)
{
    stream.read(value);
}

// PlayerHasItemExpression

BoolExpression::Type CustomScript::PlayerHasItemExpression::getType()
{
    return etPlayerHasItem;
}

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
    AdventureObject* object = itemExp->evaluate();
    Item* item = dynamic_cast<Item*>(object);
    if (!item)
        throw(EItemTypeConflict, object);
    return getAction()->getPlayerInv()->hasItem(item);
    
}

bool PlayerHasItemExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::string playerHasExp("player has");

    PlayerHasItemExpression* typed = new PlayerHasItemExpression(data.script);

    if (!quick_check(data.bounds, playerHasExp))
    {
        delete typed;
        return true;
    }
    data.bounds.advance(playerHasExp.size());

    typed->itemExp = ObjectExpression::TryParse(data);
    if (!typed->itemExp)
    {
        delete typed;
        return false;
    }

    expr = typed;
    return true;
}

void CustomScript::PlayerHasItemExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    itemExp->save(stream);
}

void CustomScript::PlayerHasItemExpression::load(FileStream & stream, Script * script)
{
    itemExp = ObjectExpression::loadTyped(stream, script);
}

// BracketExpression

BoolExpression::Type CustomScript::BracketExpression::getType()
{
    return etBracket;
}

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
    static const std::string openingBracket("(");
    static const std::string closingBracket(")");
    
    if (!quick_check(data.bounds, openingBracket))
        return true;
    data.bounds.advance(openingBracket.size());
    expr = BoolExpression::TryParse(data);
    if (!expr)
        return false;
    if (!quick_check(data.bounds, closingBracket))
    {
        data.script->error("Closing bracket \")\" expected!");
        delete expr;
        return false;
    }
    data.bounds.advance(closingBracket.size());

    return true;
}

void CustomScript::BracketExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    boolExp->save(stream);
}

void CustomScript::BracketExpression::load(FileStream & stream, Script * script)
{
    boolExp = BoolExpression::loadTyped(stream, script);
}

// LogicNotExpression

BoolExpression::Type CustomScript::LogicNotExpression::getType()
{
    return etLogicNot;
}

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
    static const std::string notExp("not");

    LogicNotExpression* typed = new LogicNotExpression(data.script);

    if (!quick_check(data.bounds, notExp))
    {
        delete typed;
        return true;
    }
    data.bounds.advance(notExp.size());

    typed->boolExp = BoolExpression::TryParse(data);
    if (!typed->boolExp)
    {
        delete typed;
        return false;
    }

    expr = typed;
    return true;
}

void CustomScript::LogicNotExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    boolExp->save(stream);
}

void CustomScript::LogicNotExpression::load(FileStream & stream, Script * script)
{
    boolExp = BoolExpression::loadTyped(stream, script);
}

// LogicOpExpression

BoolExpression::Type CustomScript::LogicOpExpression::getType()
{
    return etLogicOp;
}

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
    throw(ENotImplemented, "Evaluation of operation " + std::to_string(operation));
}

bool LogicOpExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    static const std::string operatorAndExp("and");
    static const std::string operatorOrExp("or");
    static const std::string operatorXorExp("xor");
    
    LogicOpExpression* typed = new LogicOpExpression(data.script);

    data.skipLogicOp = true;
    typed->boolExp1 = BoolExpression::TryParse(data);
    if (!typed->boolExp1)
    {
        delete typed;
        return true;
    }

    if (quick_check(data.bounds, operatorAndExp))
    {
        data.bounds.advance(operatorAndExp.size());
        typed->operation = opAND;
    }
    else if (quick_check(data.bounds, operatorOrExp))
    {
        data.bounds.advance(operatorOrExp.size());
        typed->operation = opOR;
    }
    else if (quick_check(data.bounds, operatorXorExp))
    {                                              
        data.bounds.advance(operatorXorExp.size());
        typed->operation = opXOR;
    }
    else
    {
        delete typed;
        return true;
    }
    
    typed->boolExp2 = BoolExpression::TryParse(data);
    if (!typed->boolExp2)
    {
        delete typed;
        return false;
    }

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

void CustomScript::LogicOpExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    boolExp1->save(stream);
    boolExp2->save(stream);
    stream.write(static_cast<byte>(operation));
}

void CustomScript::LogicOpExpression::load(FileStream & stream, Script * script)
{
    boolExp1 = BoolExpression::loadTyped(stream, script);
    boolExp2 = BoolExpression::loadTyped(stream, script);
    operation = static_cast<LogicalOperation>(stream.readByte());
}

// LocationHasItemExpression

BoolExpression::Type CustomScript::LocationHasItemExpression::getType()
{
    return etLocationHasItem;
}

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
    AdventureObject* object = locationExp->evaluate();
    Location* location = dynamic_cast<Location*>(object);
    if (!location)
        throw(ELocationTypeConflict, object);
    object = itemExp->evaluate();
    Item* item = dynamic_cast<Item*>(object);
    if (!item)
        throw(EItemTypeConflict, object);

    std::string preposition = prepositionExp->evaluate();
    try
    {
        return location->getInventory(preposition)->hasItem(item);
    }
    catch (EPrepositionNotFound)
    {
        throw(EPrepositionMissing, location, preposition);
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
        return true;
    }

    typed->locationExp = ObjectExpression::TryParse(data);
    if (!typed->locationExp)
    {
        delete typed;
        return true;
    }

    expr = typed;
    return true;
}

void CustomScript::LocationHasItemExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    locationExp->save(stream);
    itemExp->save(stream);
    prepositionExp->save(stream);
}

void CustomScript::LocationHasItemExpression::load(FileStream & stream, Script * script)
{
    locationExp = ObjectExpression::loadTyped(stream, script);
    itemExp = ObjectExpression::loadTyped(stream, script);
    prepositionExp = static_cast<IdentAsStringExpression*>(StringExpression::loadTyped(stream, script));
}

// ObjectFlagTestExpression

BoolExpression::Type CustomScript::ObjectFlagTestExpression::getType()
{
    return etObjectFlagTest;
}

CustomScript::ObjectFlagTestExpression::ObjectFlagTestExpression(Script * script)
    : BoolExpression(script)
{     
    objectExp = NULL;
    flagExp = NULL;
}

CustomScript::ObjectFlagTestExpression::~ObjectFlagTestExpression()
{
    delete objectExp;
    delete flagExp;
}

bool CustomScript::ObjectFlagTestExpression::evaluate()
{
    return objectExp->evaluate()->testFlag(flagExp->evaluate());
}

bool CustomScript::ObjectFlagTestExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    ObjectFlagTestExpression* typed = new ObjectFlagTestExpression(data.script);

    typed->objectExp = ObjectExpression::TryParse(data);
    if (!typed->objectExp)
    {
        delete typed;
        return true;
    }

    typed->flagExp = IdentAsStringExpression::TryParse(data);
    if (!typed->flagExp)
    {
        delete typed;
        return true;
    }

    expr = typed;
    return true;
}

void CustomScript::ObjectFlagTestExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    objectExp->save(stream);
    flagExp->save(stream);
}

void CustomScript::ObjectFlagTestExpression::load(FileStream & stream, Script * script)
{
    objectExp = ObjectExpression::loadTyped(stream, script);
    flagExp = static_cast<IdentAsStringExpression*>(StringExpression::loadTyped(stream, script));
}

// GlobalFlagTestExpression

BoolExpression::Type CustomScript::GlobalFlagTestExpression::getType()
{
    return etGlobalFlagTest;
}

CustomScript::GlobalFlagTestExpression::GlobalFlagTestExpression(Script * script)
    : BoolExpression(script)
{
    flagExp = NULL;
}

CustomScript::GlobalFlagTestExpression::~GlobalFlagTestExpression()
{
    delete flagExp;
}

bool CustomScript::GlobalFlagTestExpression::evaluate()
{
    return getAction()->getAdventure()->testFlag(flagExp->evaluate());
}

bool CustomScript::GlobalFlagTestExpression::TryParse(ParseData & data, BoolExpression *& expr)
{
    GlobalFlagTestExpression* typed = new GlobalFlagTestExpression(data.script);

    typed->flagExp = IdentAsStringExpression::TryParse(data);
    if (!typed->flagExp)
    {
        delete typed;
        return true;
    }

    expr = typed;
    return true;
}

void CustomScript::GlobalFlagTestExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    flagExp->save(stream);
}

void CustomScript::GlobalFlagTestExpression::load(FileStream & stream, Script * script)
{
    flagExp = static_cast<IdentAsStringExpression*>(StringExpression::loadTyped(stream, script));
}

// Statement

const Statement::TryParseFunc Statement::TryParseList[] = {
    IfStatement::TryParse,
    WhileStatement::TryParse,
    RepeatUntilStatement::TryParse,
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

LoopStatement * CustomScript::Statement::getLoopParent(bool setExitFlag)
{
    ControlStatement* p = getParent();
    while (p)
    {
        LoopStatement* l = dynamic_cast<LoopStatement*>(p);
        if (l) 
            return l;
        if (setExitFlag)
            p->doExit();
        p = p->getParent();
    }       
    return NULL;
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
                if (data.bounds.pos == data.bounds.end)
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

Statement * CustomScript::Statement::loadTyped(FileStream & stream, Script * script)
{
    Statement* stmt = NULL;
    switch (static_cast<Type>(stream.readByte()))
    {
    case stStatement:
        stmt = new Statement();
        break;
    case stIf:
        stmt = new IfStatement();
        break;
    case stSwitch:
        stmt = new SwitchStatement();
        break;
    case stWhile:
        stmt = new WhileStatement();
        break;
    case stDoWhile:
        stmt = new RepeatUntilStatement();
        break;
    case stBreak:
        stmt = new BreakStatement();
        break;
    case stContinue:
        stmt = new ContinueStatement();
        break;
    case stSkip:
        stmt = new SkipStatement();
        break;
    case stProcedure:
        stmt = new ProcedureStatement();
        break;
    }
    stmt->load(stream, script);
    return stmt;
}

Statement::Type Statement::getType()
{
    return stStatement;
}

void CustomScript::Statement::save(FileStream & stream)
{
    stream.write(static_cast<byte>(getType()));
    stream.write(next != NULL);
    if (next)
        next->save(stream);
}

void CustomScript::Statement::load(FileStream & stream, Script * script)
{
    this->script = script;
    if (stream.readBool())
        next = loadTyped(stream, script);
    else
        next = NULL;
}                       

// ControlStatement

bool CustomScript::ControlStatement::exitOccured()
{
    return exitFlag;
}

void CustomScript::ControlStatement::doExit()
{
    exitFlag = true;
}

void CustomScript::ControlStatement::preExecute()
{
    exitFlag = false;
}

bool CustomScript::ControlStatement::execute()
{
    return !exitFlag && Statement::execute();
}

// ControlStatement

bool ConditionalStatement::evaluateCondition()
{
    return condition->evaluate();
}

ConditionalStatement::ConditionalStatement()
{
    condition = NULL;
}

ConditionalStatement::~ConditionalStatement()
{
    delete condition;
}

void CustomScript::ConditionalStatement::save(FileStream & stream)
{
    Statement::save(stream);
    condition->save(stream);
}

void CustomScript::ConditionalStatement::load(FileStream & stream, Script * script)
{
    Statement::load(stream, script);
    condition = BoolExpression::loadTyped(stream, script);
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
    preExecute();
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
    return success && ControlStatement::execute();
}

bool IfStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::string ifExp("if");
    static const std::string thenExp("then");
    static const std::string elseExp("else");
    static const std::string elseifExp("elseif");
    static const std::string endExp("end");

    if (!quick_check(data.bounds, ifExp))
        return true;

    data.bounds.advance(ifExp.size());

    BoolExpression* condition = BoolExpression::TryParse(data);
    if (!condition)
    {
        data.script->error("Boolean expression expected!");
        return false;
    }

    if (!quick_check(data.bounds, thenExp))
    {
        data.script->error("Then expected!");
        delete condition;
        return false;
    }
    data.bounds.advance(thenExp.size());

    IfStatement* typed = new IfStatement();
    stmt = typed;

    typed->setScript(data.script);
    typed->setParent(data.parent);
    typed->condition = condition;

    Statement* thenPart = new Statement();
    typed->thenPart = thenPart;
    if (thenPart->parse(data, typed) == prError)
        return false;

    IfStatement* lastElseif = typed;
    while (quick_check(data.bounds, elseifExp))
    {
        data.bounds.advance(elseifExp.size());
        IfStatement* then = new IfStatement();
        then->setScript(data.script);
        then->setParent(data.parent);
        then->condition = BoolExpression::TryParse(data);

        if (!then->condition)
        {
            data.script->error("Condition expected!");
            return false;
        }

        if (!quick_check(data.bounds, thenExp))
        {
            data.script->error("Then expected!");
            delete condition;
            return false;
        }
        data.bounds.advance(thenExp.size());

        lastElseif->elsePart = then;
        lastElseif = then;

        then->thenPart = new Statement();
        if (then->thenPart->parse(data, typed) == prError)
            return false;
    }

    if (quick_check(data.bounds, elseExp))
    {
        data.bounds.advance(elseExp.size());
        Statement* elsePart = new Statement();
        lastElseif->elsePart = elsePart;
        if (elsePart->parse(data, typed) == prError)
            return false;
    }

    if (quick_check(data.bounds, endExp))
    {
        data.bounds.advance(endExp.size());
        return true;
    }

    data.script->error("End expected!");
    return false;
}

Statement::Type CustomScript::IfStatement::getType()
{
    return stIf;
}

void CustomScript::IfStatement::save(FileStream & stream)
{
    ConditionalStatement::save(stream);
    thenPart->save(stream);
    stream.write(elsePart != NULL);
    if (elsePart)
        elsePart->save(stream);
}

void CustomScript::IfStatement::load(FileStream & stream, Script * script)
{
    ConditionalStatement::load(stream, script);
    thenPart = loadTyped(stream, script);
    if (stream.readBool())
        elsePart = loadTyped(stream, script);
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
    preExecute();
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
    static const std::string caseExp("case");
    static const std::string ofExp("of");
    static const std::string labelExp(":");
    static const std::string elseExp("else");
    static const std::string endExp("end");

    if (!quick_check(data.bounds, caseExp))
        return true;
    data.bounds.advance(caseExp.size());

    StringExpression* switchPart = NULL;
    if (!ParamExpression::TryParse(data, switchPart))
        return false;
    if (!switchPart)
    {
        data.script->error("Paremeter expression expected");
        return false;
    }

    if (!quick_check(data.bounds, ofExp))
    {
        data.script->error("Of expected");
        return false;
    }
    data.bounds.advance(ofExp.size());

    SwitchStatement* typed = new SwitchStatement();
    typed->switchPart = (ParamExpression*)switchPart;
    typed->setScript(data.script);
    typed->setParent(data.parent);

    while (quick_check(data.bounds, labelExp))
    {
        data.bounds.advance(labelExp.size());
        ObjectExpression* expr = NULL;
        if (!IdentExpression::TryParse(data, expr))
            return false;
        if (!expr)
        {
            data.script->error("Ident expression expected");
            delete typed;
            return false;
        }

        IdentExpression* iexpr = (IdentExpression*)expr;

        Statement* caseStmt = new Statement();
        typed->caseParts.push_back(CaseSection(static_cast<IdentExpression*>(expr), caseStmt));
        if (caseStmt->parse(data, typed) == prError)
        {
            delete typed;
            return false;
        }
    }

    if (quick_check(data.bounds, elseExp))
    {
        data.bounds.advance(elseExp.size());
        Statement* elseStatement = new Statement();
        typed->elsePart = elseStatement;
        if (elseStatement->parse(data, typed) == prError)
        {
            delete typed;
            return false;
        }
    }

    if (!quick_check(data.bounds, endExp))
    {
        data.script->error("End expected");
        delete typed;
        return false;
    }
    data.bounds.advance(endExp.size());

    stmt = typed;
    return true;
}

Statement::Type CustomScript::SwitchStatement::getType()
{
    return stSwitch;
}

void CustomScript::SwitchStatement::save(FileStream & stream)
{
    Statement::save(stream);
    switchPart->save(stream);
    stream.write(static_cast<UINT>(caseParts.size()));
    for (const CaseSection & caseSection : caseParts)
    {
        caseSection.ident->save(stream);
        caseSection.statement->save(stream);
    }
    if (elsePart)
        elsePart->save(stream);
}

void CustomScript::SwitchStatement::load(FileStream & stream, Script * script)
{
    Statement::load(stream, script);
    switchPart = static_cast<ParamExpression*>(StringExpression::loadTyped(stream, script));
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
    {
        IdentExpression* identExp = static_cast<IdentExpression*>(ObjectExpression::loadTyped(stream, script));
        Statement* stmt = loadTyped(stream, script);        
        caseParts.push_back(CaseSection(identExp, stmt));
    }
    if (stream.readBool())
        elsePart = loadTyped(stream, script);
}

// LoopStatement

void CustomScript::LoopStatement::setLoopPart(Statement * loopPart)
{
    this->loopPart = loopPart;
}

bool LoopStatement::executeLoopPart()
{
    return loopPart && loopPart->execute() || !loopPart;
}

bool LoopStatement::breakOccured()
{
    return breakFlag;
}

bool CustomScript::LoopStatement::continueOccured()
{
    return continueFlag;
}

void LoopStatement::preExecute()
{
    ControlStatement::preExecute();
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

void CustomScript::LoopStatement::doContinue()
{
    continueFlag = true;
}

void CustomScript::LoopStatement::save(FileStream & stream)
{
    ConditionalStatement::save(stream);
    loopPart->save(stream);
}

void CustomScript::LoopStatement::load(FileStream & stream, Script * script)
{
    ConditionalStatement::load(stream, script);
    loopPart = loadTyped(stream, script);
}

// WhileStatement

bool WhileStatement::execute()
{
    preExecute();
    bool success = true;
    while (!breakOccured() && success && evaluateCondition())
        success = executeLoopPart();
    return success && ConditionalStatement::execute();
}

bool WhileStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::string whileExp("while");
    static const std::string doExp("do");
    static const std::string endExp("end");

    if (!quick_check(data.bounds, whileExp))
        return true;

    data.bounds.advance(whileExp.size());

    BoolExpression* condition = BoolExpression::TryParse(data);
    if (!condition)
    {
        data.script->error("Boolean expression expected!");
        return false;
    }

    if (!quick_check(data.bounds, doExp))
    {
        data.script->error("Do expected!");
        delete condition;
        return false;
    }
    data.bounds.advance(doExp.size());

    WhileStatement* typed = new WhileStatement();
    
    typed->setScript(data.script);
    typed->setParent(data.parent);
    typed->condition = condition;

    Statement* loopPart = new Statement();
    typed->setLoopPart(loopPart);
    if (loopPart->parse(data, typed) == prError)
    {
        delete typed;
        return false;
    }

    if (!quick_check(data.bounds, endExp))
    {
        data.script->error("End expected");
        delete typed;
        return false;
    }
    data.bounds.advance(endExp.size());

    stmt = typed;

    return true;
}

Statement::Type CustomScript::WhileStatement::getType()
{
    return stWhile;
}

// RepeatUntilStatement

bool RepeatUntilStatement::execute()
{
    preExecute();
    bool success;
    do
    {
        success = executeLoopPart();
    } while (!breakOccured() && (success || continueOccured()) && !evaluateCondition());
    return success && ConditionalStatement::execute();
}

bool RepeatUntilStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::string repeatExp("repeat");
    static const std::string untilExp("until");

    if (!quick_check(data.bounds, repeatExp))
        return true;

    data.bounds.advance(repeatExp.size());

    RepeatUntilStatement* typed = new RepeatUntilStatement();
    
    typed->setScript(data.script);
    typed->setParent(data.parent);

    Statement* loopPart = new Statement();
    typed->setLoopPart(loopPart);
    if (loopPart->parse(data, typed) == prError)
        return false;           

    if (!quick_check(data.bounds, untilExp))
    {
        data.script->error("Until expected!");
        delete typed;
        return false;
    }
    data.bounds.advance(untilExp.size());

    BoolExpression* condition = BoolExpression::TryParse(data);
    if (!condition)
    {
        data.script->error("Boolean expression expected!");
        delete typed;
        return false;
    }
    typed->condition = condition;

    stmt = typed;

    return true;
}

Statement::Type CustomScript::RepeatUntilStatement::getType()
{
    return stDoWhile;
}

// BreakStatement

bool BreakStatement::execute()
{
    if (LoopStatement* loop = getLoopParent(true))
        loop->doBreak();
    return true;
}

bool BreakStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::string breakExp("break");

    std::smatch matches;
    if (!quick_check(data.bounds, breakExp))
        return true;
    data.bounds.advance(breakExp.size());
    stmt = new BreakStatement();
    stmt->setParent(data.parent);
    stmt->setScript(data.script);

    if (!stmt->getLoopParent())
    {
        data.script->error("Break must be inside of a loop!");
        return false;
    }

    return true;
}

Statement::Type CustomScript::BreakStatement::getType()
{
    return stBreak;
}

// ContinueStatement

bool ContinueStatement::execute()
{
    if (LoopStatement* loop = getLoopParent(true))
        loop->doContinue();
    return true;
}

bool ContinueStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::string continueExp("continue");

    std::smatch matches;
    if (!quick_check(data.bounds, continueExp))
        return true;
    data.bounds.advance(continueExp.size());
    stmt = new ContinueStatement();
    stmt->setParent(data.parent);
    stmt->setScript(data.script);

    if (!stmt->getLoopParent())
    {
        data.script->error("Continue must be inside of a loop!");
        return false;
    }

    return true;
}

Statement::Type CustomScript::ContinueStatement::getType()
{
    return stContinue;
}

// SkipStatement

bool SkipStatement::execute()
{
    return false;
}

bool SkipStatement::TryParse(ParseData & data, Statement *& stmt)
{
    static const std::string skipExp("skip");

    std::smatch matches;
    if (!quick_check(data.bounds, skipExp))
        return true;
    data.bounds.advance(skipExp.size());
    stmt = new SkipStatement();
    stmt->setParent(data.parent);
    stmt->setScript(data.script);
    return true;
}

Statement::Type CustomScript::SkipStatement::getType()
{
    return stSkip;
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

    ProcedureData("inform",{ etObject }), // player_inform object
    ProcedureData("forget",{ etObject }), // player_forget object    

    ProcedureData("lock",{ etObject }), // lock room_connection
    ProcedureData("unlock",{ etObject }), // unlock room_connection

    ProcedureData("add_item_combination",{ etObject, etObject, etObject }), // add_item_combination item1, item2, item_out
    ProcedureData("del_item_combination",{ etObject, etObject }),           // del_item_combination item1, item2 

    ProcedureData("set",{ etObject, etIdent }),    // set object, flag
    ProcedureData("clear",{ etObject, etIdent }),  // clear object, flag
    ProcedureData("toggle",{ etObject, etIdent }), // toggle object, flag
    ProcedureData("g_set",{ etIdent }),       // set flag
    ProcedureData("g_clear",{ etIdent }),     // clear flag
    ProcedureData("g_toggle",{ etIdent }),    // toggle flag

    ProcedureData("run_with", {etObject, etString})    // run_customcommand object, command
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
    auto getObject = [&](int param)
    {
        return static_cast<ObjectExpression*>(params[param])->evaluate();
    };

    auto getRoom = [&](int param)
    {
        AdventureObject* object = getObject(param);
        Room* room = dynamic_cast<Room*>(object);
        if (!room)
            throw(ERoomTypeConflict, object);
        return room;
    };

    auto getItem = [&](int param)
    {
        AdventureObject* object = getObject(param);
        Item* item = dynamic_cast<Item*>(object);
        if (!item)
            throw(EItemTypeConflict, object);
        return item;
    };

    auto getLocation = [&](int param)
    {
        AdventureObject* object = getObject(param);
        Location* location = dynamic_cast<Location*>(object);
        if (!location)
            throw(ELocationTypeConflict, object);
        return location;
    };

    auto getRoomConnection = [&](int param)
    {
        AdventureObject* object = getObject(param);
        RoomConnection* roomConnection = dynamic_cast<RoomConnection*>(object);
        if (!roomConnection)
            throw(ELocationTypeConflict, object);
        return roomConnection;
    };

    auto getString = [&](int param)
    {
        return static_cast<StringExpression*>(params[param])->evaluate();
    };

    switch (type)
    {
    case ptWrite: {
        getAction()->write(getString(0));
        break;
    }
    case ptDraw: {
        throw(ENotImplemented, "AdventureScript Draw-Procedure");
        break;
    }            

    case ptSetRoom: {
        getAction()->getPlayer()->gotoRoom(getRoom(0));
        break;
    }
    case ptSetLocation: {
        Location* location = getLocation(0);
        if (!getAction()->currentRoom()->hasLocation(location))
            throw(ELocationMissing, location, getAction()->currentRoom());
        getAction()->getPlayer()->gotoLocation(location);
        break;
    }                
    case ptPlayerAddItem: {
        getAction()->getPlayerInv()->addItem(getItem(0));
        break;
    }
    case ptPlayerDelItem: {
        getAction()->getPlayerInv()->delItem(getItem(0));
        break;
    }
    case ptLocationAddItem: {
        getLocation(2)->getInventory(getString(1))->addItemForce(getItem(0));
        break;
    }
    case ptLocationDelItem: {        
        getLocation(2)->getInventory(getString(1))->delItem(getItem(0));
        break;
    }

    case ptFilterAdd: {
        getLocation(2)->getInventory(getString(1))->addToFilter(getItem(0));
        break;
    }
    case ptFilterDel: {
        getLocation(2)->getInventory(getString(1))->delFromFilter(getItem(0));
        break;
    }
    case ptFilterWhitelist: {
        getLocation(1)->getInventory(getString(0))->enableFilter(Location::PInventory::ifWhitelist);
        break;
    }
    case ptFilterBlacklist: {
        getLocation(1)->getInventory(getString(0))->enableFilter(Location::PInventory::ifBlacklist);
        break;
    }
    case ptFilterDisable: {
        getLocation(1)->getInventory(getString(0))->disableFilter();
        break;
    }

    case ptSetDescription: {
        getObject(0)->setDescription(getString(1));
        break;
    }
    case ptAddAlias: {
        getObject(0)->getAliases().add(getString(1));
        break;
    }
    case ptDelAlias: {
        getObject(0)->getAliases().del(getString(1));
        break;
    }

    case ptPlayerInform: {
        getAction()->getPlayer()->inform(getObject(0));
        break;
    }
    case ptPlayerForget: {
        getAction()->getPlayer()->forget(getObject(0));
        break;
    }

    case ptLock: {
        getRoomConnection(0)->lock();
        break;
    }
    case ptUnlock: {
        getRoomConnection(0)->unlock();
        break;
    }

    case ptAddItemCombination: {
        getAction()->getItemCombiner()->addCombination(getItem(0), getItem(1), getItem(2));
        break;
    }
    case ptDelItemCombination: {
        getAction()->getItemCombiner()->delCombination(getItem(0), getItem(1)); 
        break;
    }

    case ptSet: {
        getObject(0)->setFlag(getString(1));
        break;
    }
    case ptClear: {
        getObject(0)->clearFlag(getString(1));
        break;
    }       
    case ptToggle: {
        getObject(0)->toggleFlag(getString(1));
        break;
    }
    case ptGlobalSet: {
        getAction()->getAdventure()->setFlag(getString(0));
        break;
    }
    case ptGlobalClear: {
        getAction()->getAdventure()->clearFlag(getString(0));
        break;
    }
    case ptGlobalToggle: {
        getAction()->getAdventure()->toggleFlag(getString(0));
        break;
    }
                
    case ptRunWith: {
        AdventureObject* object = getObject(0);
        std::string cmd = getString(1);
        if (Room* room = dynamic_cast<Room*>(object))
        {
            room->getLocatedCommands()->sendCommand(cmd);
        }
        else if (Location* location = dynamic_cast<Location*>(object))
        {
            location->getLocatedCommands()->sendCommand(cmd);
        } 
        else if (Item* item = dynamic_cast<Item*>(object))
        {
            item->getCarryCommands()->sendCommand(cmd);
        }
        else
        {
            throw(ERunWithUnknownObjectType, object);
        }
        break;
    }
    }
    return Statement::execute();
}

bool ProcedureStatement::TryParse(ParseData & data, Statement *& stmt)
{
    std::string ident;
    if (!parse_ident(data.bounds, ident))
        return true;

    bool found = false;
    ProcedureType funcType;
    for (funcType = ptWrite; funcType < PROCEDURE_COUNT; funcType = (ProcedureType)(funcType + 1))
    {
        if (Functions[funcType].name == ident)
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

    data.bounds.advance(ident.size());

    for (ExpressionType exprType : Functions[typed->type].params)
    {
        Expression* expr = NULL;
        switch (exprType)
        {
        case etObject:
            expr = ObjectExpression::TryParse(data);
            if (!expr)
                data.script->error("Object expression excpected");
            break;
        case etBool:
            expr = BoolExpression::TryParse(data);
            if (!expr)
                data.script->error("Boolean expression excpected");
            break;
        case etString:
            expr = StringExpression::TryParse(data);
            if (!expr)
                data.script->error("String expression excpected");
            break;
        case etIdent:
            expr = IdentAsStringExpression::TryParse(data);
            if (!expr)
                data.script->error("Ident expression excpected");
            break;
        }

        if (!expr)
            return false;
        typed->params.push_back(expr);
    }

    return true;
}

Statement::Type CustomScript::ProcedureStatement::getType()
{
    return stProcedure;
}

void CustomScript::ProcedureStatement::save(FileStream & stream)
{
    Statement::save(stream);
    stream.write(static_cast<byte>(type));
    for (Expression* param : params)
        param->save(stream);
}

void CustomScript::ProcedureStatement::load(FileStream & stream, Script * script)
{
    Statement::load(stream, script);
    type = static_cast<ProcedureType>(stream.readByte());
    for (ExpressionType paramType : Functions[type].params)
    {
        params.push_back(Expression::loadTyped(stream, paramType, script));
    }
}

// Script

CustomScript::Script::Script(CustomAdventureAction* action, FileStream & stream)
{                 
    this->action = action;
    stream.read(title);
    root = Statement::loadTyped(stream, this);
    stream.read(requiredParams);
}

Script::Script(CustomAdventureAction* action, std::string code, std::string title)
{
    this->code = new std::string(code);
    parseData = new ParseData(StringBounds(*this->code), this);

    this->action = action;
    this->title = title;

    root = new Statement();
    root->setScript(this);
    if (parseData->bounds.pos == parseData->bounds.end)
        success = true;
    else
        switch (root->parse(*parseData, NULL))
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

    delete this->code;
    delete parseData;
}

CustomScript::Script::~Script()
{
    delete root;
}

bool Script::run(const Command::Result & params)
{
    this->params = &params;
    return root->execute();
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

const std::string & CustomScript::Script::getCode()
{
    return *code;
}

bool Script::succeeded() const
{
    return success;
}

void Script::error(std::string message) const
{
    size_t line = std::count(parseData->bounds.text.cbegin(), parseData->bounds.text.cbegin() + parseData->bounds.pos, '\n') + 1;
    size_t column;
    for (column = 0; column < parseData->bounds.pos; column++)
        if (parseData->bounds.text[column] == '\n')
            break;

    ErrorDialog("CustomScript Error",
        "Error in script " + title +
        " at line " + std::to_string(line) +
        " column " + std::to_string(column) +
        ":\n" + message);
}

void CustomScript::Script::save(FileStream & stream)
{
    stream.write(title);
    root->save(stream);
    stream.write(requiredParams);
}

// Global
/*
bool CustomScript::check_regex(StringBounds bounds, std::smatch & matches, const std::regex & exp)
{
    return std::regex_search(bounds.text.cbegin() + bounds.pos, bounds.text.cend(), matches, exp, std::regex_constants::match_continuous);
}
*/
bool CustomScript::quick_check(StringBounds& bounds, const std::string & word)
{
    return !bounds.text.compare(bounds.pos, word.length(), word);
}

bool CustomScript::parse_ident(StringBounds & bounds, std::string & result)
{
    size_t i = bounds.pos;
    result = "";
    while (bounds.text[i] >= 'a' && bounds.text[i] <= 'z' ||
        bounds.text[i] >= 'A' && bounds.text[i] <= 'Z' ||
        bounds.text[i] >= '0' && bounds.text[i] <= '9' ||
        bounds.text[i] == '_')
    {
        result += bounds.text[i++];
    }
    return i != bounds.pos;
}

bool CustomScript::parse_string(StringBounds & bounds, std::string & result)
{
    if (!quick_check(bounds, "\""))
    {
        return false;
    }                   
    result = "";
    bounds.pos++;
    while (bounds.pos < bounds.text.size() && bounds.text[bounds.pos] != '"')
    {
        if (bounds.text[bounds.pos] == '\n' || bounds.text[bounds.pos] == '\r')
            return false;
        
        if (bounds.text[bounds.pos] == '\\')
        {
            if (bounds.pos + 1 < bounds.text.size())
            {
                switch (bounds.text[bounds.pos + 1])
                {
                case '"':
                    result += '"';
                    break;
                case '\\':
                    result += '\\';
                    break;
                default:
                    return false;
                }
            }
            else
            {
                return false;
            }
            bounds.pos++;
        }
        else
            result += bounds.text[bounds.pos];
        bounds.pos++;
    }
    if (bounds.pos == bounds.text.size())
    {
        return false;
    }
    bounds.advance(1);
    return true;
}

void CustomScript::skipWhitespaces(StringBounds& bounds)
{
    bool more = true;
    
    while (more)
    {
        // skip whitespaces

        if (bounds.text[bounds.pos] == ' ' ||
            bounds.text[bounds.pos] == '\n' ||
            bounds.text[bounds.pos] == '\t' ||
            bounds.text[bounds.pos] == '\r')
        {
            do
            {
                bounds.pos++;
            } while (bounds.text[bounds.pos] == ' ' ||
                bounds.text[bounds.pos] == '\n' ||
                bounds.text[bounds.pos] == '\t' ||
                bounds.text[bounds.pos] == '\r');

            continue;
        }

        // ignore // comment
        if (quick_check(bounds, "//"))
        {
            bounds.pos = bounds.text.find('\n', bounds.pos + 2);
            continue;
        }

        // ignore { comment }
        if (quick_check(bounds, "{"))
        {
            bounds.pos = bounds.text.find('}', bounds.pos + 2);
            continue;
        }

        more = false;
    }
}

CustomScript::EScript::EScript(std::string msg)
    : Exception(msg)
{
}

CustomScript::ECompile::ECompile(std::string msg)
    : EScript("Script Compilation Error: " + msg)
{
}

CustomScript::ERuntime::ERuntime(std::string msg)
    : EScript("Script Runtime Error: " + msg)
{
}

CustomScript::EObjectEvaluation::EObjectEvaluation(std::string identifier)
    : ERuntime("Could not evaluate \"" + identifier + "\"")
{
}

CustomScript::EUnknownParam::EUnknownParam(std::string param)
    : ERuntime("Unknown parameter \"" + param + "\"")
{
}

CustomScript::EObjectTypeConflict::EObjectTypeConflict(const AdventureObject * object, std::string expectedType)
    : ERuntime("Object typed conflict: " + object->getName() + " should be " + expectedType)
{
}

CustomScript::EItemTypeConflict::EItemTypeConflict(const AdventureObject * object)
    : EObjectTypeConflict(object, "item")
{
}

CustomScript::ERoomTypeConflict::ERoomTypeConflict(const AdventureObject * object)
    : EObjectTypeConflict(object, "room")
{
}

CustomScript::ELocationTypeConflict::ELocationTypeConflict(const AdventureObject * object)
    : EObjectTypeConflict(object, "location")
{
}

CustomScript::EPrepositionMissing::EPrepositionMissing(const Location * location, std::string preposition)
    : ERuntime("Location \"" + location->getName() + "\" is missing preposition \"" + preposition + "\"")
{
}

CustomScript::ELocationMissing::ELocationMissing(const Location * location, const Room * room)
    : ERuntime("Location \"" + location->getName() + "\" missing in room \"" + room->getName() + "\"")
{
}

CustomScript::ERunWithUnknownObjectType::ERunWithUnknownObjectType(AdventureObject * object)
    : Exception("Object \"" + object->getName() + "\" does not have CustomCommands and can therefore not be used with run_with")
{
}
