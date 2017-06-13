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

ParseData::ParseData(StringBounds bounds, Script & script)
    : bounds(bounds)
    , script(script)
    , skipLogicOp(false)
{
}

// Expression

CustomScript::Expression::Expression(Script & script)
    : script(script)
{
}

CustomScript::Expression::~Expression()
{
}

Script & Expression::getScript() const
{
    return script;
}

const Command::Result & Expression::getParams() const
{
    return script.getParams();
}

CustomAdventureAction & Expression::getAction() const
{
    return script.getAction();
}

Expression & CustomScript::Expression::loadTyped(FileStream & stream, ExpressionType type, Script & script)
{
    switch (type)
    {                  
    case etObject:
        return ObjectExpression::loadTyped(stream, script);
    case etBool:
        return BoolExpression::loadTyped(stream, script);
    case etString:
    case etIdent:
        return StringExpression::loadTyped(stream, script);
    }
    throw(ENotImplemented, "CustomScript Expression Type " + std::to_string(type));
}

// ObjectExpression                     

const ObjectExpression::TryParseFunc ObjectExpression::TryParseList[] = {
    IdentExpression::TryParse
};

CustomScript::ObjectExpression::ObjectExpression(Script & script)
    : TypedExpression<AdventureObject&>(script)
{
}

ObjectExpression * ObjectExpression::TryParse(ParseData & data)
{
    for (TryParseFunc func : TryParseList)
    {
        // save the current position in the code
        size_t oldPos = data.bounds.pos;        
        ObjectExpression * result = func(data);
        if (result)
            return result;
        
        // revert if no match
        data.bounds.pos = oldPos;                                               
    }
    // no expression fit
    return NULL;
}

ExpressionType ObjectExpression::getResultType()
{
    return etObject;
}

ObjectExpression & CustomScript::ObjectExpression::loadTyped(FileStream & stream, Script & script)
{
    Type type = static_cast<Type>(stream.readByte());
    switch (type)
    {
    case etIdent:
        return *new IdentExpression(stream, script);           
    }
    throw(ENotImplemented, "CustomScript ObjectExpression Type " + std::to_string(type));
}

void CustomScript::ObjectExpression::save(FileStream & stream)
{
    stream.write(static_cast<byte>(getType()));
}

// StringExpression 

CustomScript::StringExpression::StringExpression(Script & script)
    : TypedExpression<std::string>(script)
{
}

StringExpression * StringExpression::TryParse(ParseData & data)
{
    return StringConcatExpression::TryParse(data);
}

ExpressionType StringExpression::getResultType()
{
    return etString;
}

StringExpression & CustomScript::StringExpression::loadTyped(FileStream & stream, Script & script)
{
    Type type = static_cast<Type>(stream.readByte());
    switch (type)
    {
    case etObjectToString:
        return *new ObjectToStringExpression(stream, script);
    case etConstString:
        return *new ConstStringExpression(stream, script);
    case etStringConcat:
        return *new StringConcatExpression(stream, script);
    case etParam:
        return *new ParamExpression(stream, script);
    case etIdentAsString:
        return *new IdentAsStringExpression(stream, script);
    }
    throw(ENotImplemented, "CustomScript StringExpression Type " + std::to_string(type));
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

CustomScript::BoolExpression::BoolExpression(Script & script)
    : TypedExpression<bool>(script)
{
}

BoolExpression * BoolExpression::TryParse(ParseData & data)
{
    BoolExpression * expr = NULL;
    for (TryParseFunc func : TryParseList)
    {
        if (data.skipLogicOp && func == LogicOpExpression::TryParse)
        {
            data.skipLogicOp = false;
            continue;
        }
        size_t oldPos = data.bounds.pos;
        BoolExpression * result = func(data);
        if (result)
            return result;
        data.bounds.pos = oldPos;        
        
    }
    return NULL;
}

ExpressionType BoolExpression::getResultType()
{
    return etBool;
}

BoolExpression & CustomScript::BoolExpression::loadTyped(FileStream & stream, Script & script)
{
    Type type = static_cast<Type>(stream.readByte());
    switch (type)
    {
    case etParamIsIdent:
        return *new ParamIsIdentExpression(stream, script);
    case etConstBool:
        return *new ConstBoolExpression(stream, script);
    case etPlayerHasItem:
        return *new PlayerHasItemExpression(stream, script);
    case etBracket:
        return *new BracketExpression(stream, script);
    case etLogicNot:
        return *new LogicNotExpression(stream, script);
    case etLogicOp:
        return *new LogicOpExpression(stream, script);
    case etLocationHasItem:
        return *new LocationHasItemExpression(stream, script);
    case etObjectFlagTest:
        return *new ObjectFlagTestExpression(stream, script);
    case etGlobalFlagTest:
        return *new GlobalFlagTestExpression(stream, script);
    }
    throw(ENotImplemented, "CustomScript BoolExpression Type " + std::to_string(type));
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

CustomScript::IdentExpression::IdentExpression(Script & script, const std::string & identifier)
    : ObjectExpression(script)
    , identifier(identifier)
{
}

CustomScript::IdentExpression::IdentExpression(FileStream & stream, Script & script)
    : ObjectExpression(script)
    , identifier(stream.readString())
{
}

AdventureObject & IdentExpression::evaluate()
{
    try
    {
        return getAction().getAdventure().findObjectByName(identifier);
    }
    catch (EAdventureObjectNameNotFound)
    {
        throw(EObjectEvaluation, identifier);
    }
}

ObjectExpression * IdentExpression::TryParse(ParseData & data)
{
    static const std::string identPrefixExp(":");

    if (!quick_check(data.bounds, identPrefixExp))
        return NULL;

    data.bounds.pos += identPrefixExp.size();

    std::string ident;
    if (!parse_ident(data.bounds, ident))
        return NULL;

    data.bounds.advance(ident.size());

    return new IdentExpression(data.script, ident);
}

void CustomScript::IdentExpression::save(FileStream & stream)
{
    ObjectExpression::save(stream);
    stream.write(identifier);
}

// ObjectToStringExpression

StringExpression::Type CustomScript::ObjectToStringExpression::getType()
{
    return etObjectToString;
}

ObjectToStringExpression::ObjectToStringExpression(Script & script, ObjectExpression & objectExp, bool startOfSentence, GenerateType type)
    : StringExpression(script)
    , objectExp(objectExp)
    , startOfSentence(startOfSentence)
    , type(type)
{
}

CustomScript::ObjectToStringExpression::ObjectToStringExpression(FileStream & stream, Script & script)
    : StringExpression(script) 
    , objectExp(ObjectExpression::loadTyped(stream, script))
    , startOfSentence(stream.readBool())
    , type(static_cast<GenerateType>(stream.readByte()))
{
}

ObjectToStringExpression::~ObjectToStringExpression()
{
    delete &objectExp;
}

std::string ObjectToStringExpression::evaluate()
{
    AdventureObject& obj = objectExp.evaluate();
    switch (type)
    {
    case gtArticleFromPlayer:
        return obj.getName(getAction().getPlayer(), startOfSentence);
        break;
    case gtDefiniteArticle:
        return obj.getName(true, startOfSentence);
        break;
    case gtIndefiniteAricle:
        return obj.getName(false, startOfSentence);
        break;
    case gtNameOnly:
        return obj.getNameOnly(startOfSentence);
        break;
    default:
        throw ENotImplemented("Object Naming-Type " + std::to_string(type));
    }
}

StringExpression * ObjectToStringExpression::TryParse(ParseData & data)
{
    static const std::string openingBracketExp("[");
    static const std::string closingBracketExp("]");    

    ObjectExpression * objectExp = ObjectExpression::TryParse(data);

    if (!objectExp)
        return NULL;

    try
    {      
        bool startOfSentence;
        ObjectToStringExpression::GenerateType type;
        if (quick_check(data.bounds, openingBracketExp))
        {
            data.bounds.advance(openingBracketExp.size());

            if (data.bounds.pos == data.bounds.end)
                throw(ECompile, "Expected p/d/i/n but got end", data);

            char c = data.bounds.text[data.bounds.pos];
            data.bounds.advance(1);

            static const std::string typeHelp =
                "valid chars are: "
                "[p]->default "
                "[d]->the "
                "[i]->a/an "
                "[n]->no arcticle "
                "[UPPER]=capitalized";

            startOfSentence = isupper(c) != 0;
            switch (tolower(c))
            {
            case 'p':
                type = gtArticleFromPlayer;
                break;
            case 'd':
                type = gtDefiniteArticle;
                break;
            case 'i':
                type = gtIndefiniteAricle;
                break;
            case 'n':
                type = gtNameOnly;
                break;
            default:
                throw(ECompile, "Unknown Identifier-Type \"" + std::string(1, c) + "\", " + typeHelp, data);
            }

            if (!quick_check(data.bounds, closingBracketExp))
            {
                throw(ECompile, "Expected ] to enclose Identifier-Type", data);
            }
            data.bounds.advance(1);
        }
        else
        {
            startOfSentence = false;
            type = gtArticleFromPlayer;
        }

        return new ObjectToStringExpression(data.script, *objectExp, startOfSentence, type);
    }
    catch (...)
    {
        delete objectExp;
        throw;
    }
}

void CustomScript::ObjectToStringExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    objectExp.save(stream);
    stream.write(startOfSentence);
    stream.write(static_cast<byte>(type));
}

// ConstStringExpression

StringExpression::Type CustomScript::ConstStringExpression::getType()
{
    return etConstString;
}

CustomScript::ConstStringExpression::ConstStringExpression(Script & script, std::string text)
    : StringExpression(script)
    , text(text)
{
}

CustomScript::ConstStringExpression::ConstStringExpression(FileStream & stream, Script & script)
    : StringExpression(script)
    , text(stream.readString())
{
}

std::string ConstStringExpression::evaluate()
{
    return text;
}

StringExpression * ConstStringExpression::TryParse(ParseData & data)
{
    std::string text;
    if (!parse_string(data.bounds, text))
        return NULL;                                
    return new ConstStringExpression(data.script, text);    
}

void CustomScript::ConstStringExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(text);
}

// StringConcatExpression

StringExpression::Type CustomScript::StringConcatExpression::getType()
{
    return etStringConcat;
}

CustomScript::StringConcatExpression::StringConcatExpression(Script & script, ref_vector<StringExpression> stringExpList)
    : StringExpression(script)
    , stringExpList(stringExpList)
{
}

CustomScript::StringConcatExpression::StringConcatExpression(FileStream & stream, Script & script)
    : StringExpression(script)
{
    UINT length = stream.readUInt();
    for (UINT i = 0; i < length; i++)
        stringExpList.push_back(StringExpression::loadTyped(stream, script));
}

StringConcatExpression::~StringConcatExpression()
{
    for (StringExpression & expr : stringExpList)
        delete &expr;
}

std::string StringConcatExpression::evaluate()
{
    std::string result;
    for (StringExpression & exp : stringExpList)
        result += exp.evaluate();
    return result;
}

StringConcatExpression * StringConcatExpression::TryParse(ParseData & data)
{
    ref_vector<StringExpression> expList;
    bool foundFirst = false;
    while (true)
    {
        bool found = false;
        for (TryParseFunc func : TryParseList)
        {
            size_t oldPos = data.bounds.pos;
            try
            {
                StringExpression * entry = func(data);
                if (entry)
                {
                    expList.push_back(*entry);
                    found = true;
                    foundFirst = true;
                }
                else
                {
                    data.bounds.pos = oldPos;
                }
            }
            catch (...)
            {
                for (StringExpression & entry : expList)
                    delete &entry;
                throw;
            }
        }
        if (!found)
        {
            if (!foundFirst)
                return NULL;
            break;
         }
    }             
    return new StringConcatExpression(data.script, expList);
}

void CustomScript::StringConcatExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(static_cast<UINT>(stringExpList.size()));
    for (StringExpression & stringExpr : stringExpList)
        stringExpr.save(stream);
}                  

// ParamExpression

StringExpression::Type CustomScript::ParamExpression::getType()
{
    return etParam;
}

CustomScript::ParamExpression::ParamExpression(Script & script, std::string param)
    : StringExpression(script)
    , param(param)
{
}

CustomScript::ParamExpression::ParamExpression(FileStream & stream, Script & script)
    : StringExpression(script)
    , param(stream.readString())
{
}

std::string ParamExpression::evaluate()
{
    if (!getParams().hasParam(param))
        throw(EUnknownParam, param);
    return getParams()[param];
}

StringExpression * ParamExpression::TryParse(ParseData & data)
{
    static const std::string openingBracketExp("<");

    if (!quick_check(data.bounds, "<"))
        return NULL;

    data.bounds.pos += openingBracketExp.size();

    size_t end = data.bounds.text.find('>', data.bounds.pos);
    if (end == std::string::npos)
        throw(ECompile, "Expected \">\" closing bracket", data);

    std::string params = data.bounds.text.substr(data.bounds.pos, end - data.bounds.pos);

    data.script.getRequiredParams().insert(params);
    data.bounds.advance(end - data.bounds.pos + 1);

    return new ParamExpression(data.script, params);
}

void CustomScript::ParamExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(param);
}

// IdentAsStringExpression

StringExpression::Type CustomScript::IdentAsStringExpression::getType()
{
    return etIdentAsString;
}

CustomScript::IdentAsStringExpression::IdentAsStringExpression(Script & script, std::string identString)
    : StringExpression(script)
    , identString(identString)
{
}

CustomScript::IdentAsStringExpression::IdentAsStringExpression(FileStream & stream, Script & script)
    : StringExpression(script)
    , identString(stream.readString())
{
}

std::string IdentAsStringExpression::evaluate()
{
    return identString;
}

IdentAsStringExpression * IdentAsStringExpression::TryParse(ParseData & data)
{
    std::string ident;
    if (!parse_ident(data.bounds, ident))
        return NULL;

    data.bounds.advance(ident.length());

    return new IdentAsStringExpression(data.script, ident);
}

void CustomScript::IdentAsStringExpression::save(FileStream & stream)
{
    StringExpression::save(stream);
    stream.write(identString);
}

// ParamIsIdentExpression

BoolExpression::Type CustomScript::ParamIsIdentExpression::getType()
{
    return etParamIsIdent;
}

CustomScript::ParamIsIdentExpression::ParamIsIdentExpression(Script & script, StringExpression & paramExp, ObjectExpression & identExp)
    : BoolExpression(script)
    , paramExp(&paramExp)
    , identExp(&identExp)
{
}

CustomScript::ParamIsIdentExpression::ParamIsIdentExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , paramExp(NULL)
    , identExp(NULL)
{
    try
    {
        paramExp = &StringExpression::loadTyped(stream, script);
        identExp = &ObjectExpression::loadTyped(stream, script);
    }
    catch (...)
    {        
        delete paramExp;
        delete identExp;
        throw;
    }
}

ParamIsIdentExpression::~ParamIsIdentExpression()
{
    delete paramExp;
    delete identExp;
}

bool ParamIsIdentExpression::evaluate()
{
    try
    {
        return &getAction().getAdventure().findObjectByAlias(paramExp->evaluate()) == &identExp->evaluate();
    }
    catch (EAdventureObjectAliasNotFound)
    {
        return false;
    }
}

BoolExpression * ParamIsIdentExpression::TryParse(ParseData & data)
{
    static const std::string isExp("is");
              
    StringExpression * paramExp = ParamExpression::TryParse(data);

    if (!paramExp)
        return NULL;

    if (!quick_check(data.bounds, isExp))
    {
        delete paramExp;
        return NULL;
    }
    data.bounds.advance(isExp.size());

    try
    {
        ObjectExpression * identExp = IdentExpression::TryParse(data);
        if (identExp)
            return new ParamIsIdentExpression(data.script, *paramExp, *identExp); 

        return NULL;
    }
    catch (...)
    {
        delete paramExp;
        throw;
    }
}

void CustomScript::ParamIsIdentExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    paramExp->save(stream);
    identExp->save(stream);
}

// ConstBoolExpression

BoolExpression::Type CustomScript::ConstBoolExpression::getType()
{
    return etConstBool;
}

CustomScript::ConstBoolExpression::ConstBoolExpression(Script & script, bool value)
    : BoolExpression(script)
    , value(value)
{
}

CustomScript::ConstBoolExpression::ConstBoolExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , value(stream.readBool())
{
}

bool ConstBoolExpression::evaluate()
{
    return value;
}

BoolExpression * ConstBoolExpression::TryParse(ParseData & data)
{
    static const std::string boolTrueExp("true");
    static const std::string boolFalseExp("false");
    
    if (quick_check(data.bounds, boolTrueExp))
    {
        data.bounds.advance(boolTrueExp.size());
        return new ConstBoolExpression(data.script, true);
    }
    
    if (quick_check(data.bounds, boolFalseExp))
    {
        data.bounds.advance(boolFalseExp.size());
        return new ConstBoolExpression(data.script, false);
    }   

    return NULL;
}

void CustomScript::ConstBoolExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    stream.write(value);
}

// PlayerHasItemExpression

BoolExpression::Type CustomScript::PlayerHasItemExpression::getType()
{
    return etPlayerHasItem;
}

CustomScript::PlayerHasItemExpression::PlayerHasItemExpression(Script & script, ObjectExpression & itemExp)
    : BoolExpression(script)
    , itemExp(&itemExp)
{
}

CustomScript::PlayerHasItemExpression::PlayerHasItemExpression(FileStream & stream, Script & script)
    : BoolExpression(script) 
    , itemExp(&ObjectExpression::loadTyped(stream, script))
{
}

PlayerHasItemExpression::~PlayerHasItemExpression()
{
    delete itemExp;
}

bool PlayerHasItemExpression::evaluate()
{
    AdventureObject& object = itemExp->evaluate();
    try
    {
        Item& item = dynamic_cast<Item&>(object);
        return getAction().getPlayerInv().hasItem(item);
    }
    catch (std::bad_cast)
    {
        throw(EItemTypeConflict, object);
    }                                                        
}

BoolExpression * PlayerHasItemExpression::TryParse(ParseData & data)
{
    static const std::string playerHasExp("player has");

    if (!quick_check(data.bounds, playerHasExp))
        return NULL;
    
    data.bounds.advance(playerHasExp.size());

    ObjectExpression * itemExp = ObjectExpression::TryParse(data);

    if (itemExp)
        return new PlayerHasItemExpression(data.script, *itemExp);
    
    return NULL;
}

void CustomScript::PlayerHasItemExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    itemExp->save(stream);
}

// BracketExpression

BoolExpression::Type CustomScript::BracketExpression::getType()
{
    return etBracket;
}
                                
CustomScript::BracketExpression::BracketExpression(Script & script, BoolExpression & boolExp)
    : BoolExpression(script)
    , boolExp(&boolExp)
{
}

CustomScript::BracketExpression::BracketExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , boolExp(&BoolExpression::loadTyped(stream, script))
{
}

BracketExpression::~BracketExpression()
{
    delete boolExp;
}

bool BracketExpression::evaluate()
{
    return boolExp->evaluate();
}

BoolExpression * BracketExpression::TryParse(ParseData & data)
{
    static const std::string openingBracket("(");
    static const std::string closingBracket(")");
    
    if (!quick_check(data.bounds, openingBracket))
        return NULL;

    data.bounds.advance(openingBracket.size());
    BoolExpression * result = BoolExpression::TryParse(data);
    if (!result)
        throw(ECompile, "Boolean expression between brackets \"(...)\" expected", data);
    
    if (!quick_check(data.bounds, closingBracket))
    {
        delete result;
        throw(ECompile, "Closing bracket \")\" expected!", data);
    }

    data.bounds.advance(closingBracket.size());

    return result;
}

void CustomScript::BracketExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    boolExp->save(stream);
}

// LogicNotExpression

BoolExpression::Type CustomScript::LogicNotExpression::getType()
{
    return etLogicNot;
}

CustomScript::LogicNotExpression::LogicNotExpression(Script & script, BoolExpression & boolExp)
    : BoolExpression(script)
    , boolExp(&boolExp)
{
}

CustomScript::LogicNotExpression::LogicNotExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , boolExp(&BoolExpression::loadTyped(stream, script))
{
}

CustomScript::LogicNotExpression::~LogicNotExpression()
{
    delete boolExp;
}

bool CustomScript::LogicNotExpression::evaluate()
{
    return !boolExp->evaluate();
}

BoolExpression * CustomScript::LogicNotExpression::TryParse(ParseData & data)
{
    static const std::string notExp("not");       
    
    if (!quick_check(data.bounds, notExp))
        return NULL;

    data.bounds.advance(notExp.size());

    BoolExpression * condition = BoolExpression::TryParse(data);

    if (condition)
        return new LogicNotExpression(data.script, *condition);

    delete condition;
    throw(ECompile, "Boolean expression after \"not\" expected", data);
}

void CustomScript::LogicNotExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    boolExp->save(stream);
}

// LogicOpExpression

BoolExpression::Type CustomScript::LogicOpExpression::getType()
{
    return etLogicOp;
}

LogicOpExpression::LogicOpExpression(Script & script, BoolExpression & boolExp1, BoolExpression & boolExp2, LogicalOperation operation)
    : BoolExpression(script)
    , boolExp1(&boolExp1)
    , boolExp2(&boolExp2)
    , operation(operation)
{
}

CustomScript::LogicOpExpression::LogicOpExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , boolExp1(NULL)
    , boolExp2(NULL)
{
    try
    {
        boolExp1 = &BoolExpression::loadTyped(stream, script);
        boolExp2 = &BoolExpression::loadTyped(stream, script);
        operation = static_cast<LogicalOperation>(stream.readByte());
    }
    catch (...)
    {
        delete boolExp1;
        delete boolExp2;
        throw;
    }
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

BoolExpression * LogicOpExpression::TryParse(ParseData & data)
{
    static const std::string opExpression[LOGICAL_OPERATION_COUNT] = { "and", "or", "xor" };
    
    data.skipLogicOp = true;

    BoolExpression * boolExp1 = BoolExpression::TryParse(data);

    if (!boolExp1)
        return NULL;

    LogicalOperation operation = LOGICAL_OPERATION_COUNT;

    for (int i = 0; i < LOGICAL_OPERATION_COUNT; i++)
    {
        if (quick_check(data.bounds, opExpression[i]))
        {
            data.bounds.advance(opExpression[i].size());
            operation = opAND;
            break;
        }
    }             
    if (operation == LOGICAL_OPERATION_COUNT)
    {
        delete boolExp1;
        return NULL;
    }
    
    try
    {
        BoolExpression * boolExp2 = BoolExpression::TryParse(data);
        if (!boolExp2)
            throw(ECompile, "Expected boolean expression after logical operation", data);

        try
        {                                       
            LogicOpExpression & result = *new LogicOpExpression(data.script, *boolExp1, *boolExp2, operation);
            LogicOpExpression * current = &result;

            bool repeat;
            do
            {
                repeat = false;

                LogicOpExpression * next = dynamic_cast<LogicOpExpression*>(current->boolExp2);

                if (next && !(current->operation != opAND && next->operation == opAND))
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
            } while (repeat);

            return &result;
        }
        catch (...)
        {
            delete boolExp2;
            throw;
        }
    }
    catch (...)
    {
        delete boolExp1;
        throw;
    }  
}

void CustomScript::LogicOpExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    boolExp1->save(stream);
    boolExp2->save(stream);
    stream.write(static_cast<byte>(operation));
}

// LocationHasItemExpression

BoolExpression::Type CustomScript::LocationHasItemExpression::getType()
{
    return etLocationHasItem;
}

CustomScript::LocationHasItemExpression::LocationHasItemExpression(Script & script, ObjectExpression & locationExp, ObjectExpression & itemExp, StringExpression & prepositionExp)
    : BoolExpression(script)
    , locationExp(&locationExp)
    , itemExp(&itemExp)
    , prepositionExp(&prepositionExp)
{
}

CustomScript::LocationHasItemExpression::LocationHasItemExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , locationExp(NULL)
    , itemExp(NULL)
    , prepositionExp(NULL)
{
    try
    {
        locationExp = &ObjectExpression::loadTyped(stream, script);
        itemExp = &ObjectExpression::loadTyped(stream, script);
        prepositionExp = &StringExpression::loadTyped(stream, script);
    }
    catch (...)
    {
        delete locationExp;
        delete itemExp;
        delete prepositionExp;
        throw;
    }
}

LocationHasItemExpression::~LocationHasItemExpression()
{
    delete locationExp;
    delete itemExp;
    delete prepositionExp;
}

bool LocationHasItemExpression::evaluate()
{
    AdventureObject & locationObject = locationExp->evaluate();
    try
    {
        Location & location = dynamic_cast<Location&>(locationObject);

        AdventureObject & itemObject = itemExp->evaluate();
        try
        {
            Item & item = dynamic_cast<Item&>(itemObject);

            std::string preposition = prepositionExp->evaluate();
            try
            {
                return location.getInventory(preposition).hasItem(item);
            }
            catch (EPrepositionDoesNotExist)
            {
                throw(EPrepositionMissing, location, preposition);
            }
        }
        catch (std::bad_cast)
        {
            throw(EItemTypeConflict, itemObject);
        }              
    }
    catch (std::bad_cast)
    {
        throw(ELocationTypeConflict, locationObject);
    }
}

BoolExpression * LocationHasItemExpression::TryParse(ParseData & data)
{
    ObjectExpression * itemExp = ObjectExpression::TryParse(data);

    if (!itemExp)
        return NULL;

    try
    {
        StringExpression * prepositionExp = IdentAsStringExpression::TryParse(data);

        if (!prepositionExp)
        {
            delete itemExp;
            return NULL;
        }

        try
        {
            ObjectExpression * locationExp = ObjectExpression::TryParse(data);

            if (!locationExp)
            {
                delete itemExp;
                delete prepositionExp;
                return NULL;
            }

            return new LocationHasItemExpression(data.script, *locationExp, *itemExp, *prepositionExp);
        }
        catch (...)
        {
            delete prepositionExp;
            throw;
        }
    }
    catch (...)
    {
        delete itemExp;
        throw;
    }
}

void CustomScript::LocationHasItemExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    locationExp->save(stream);
    itemExp->save(stream);
    prepositionExp->save(stream);
}

// ObjectFlagTestExpression

BoolExpression::Type CustomScript::ObjectFlagTestExpression::getType()
{
    return etObjectFlagTest;
}

CustomScript::ObjectFlagTestExpression::ObjectFlagTestExpression(Script & script, ObjectExpression & objectExp, StringExpression & flagExp)
    : BoolExpression(script)
    , objectExp(&objectExp)
    , flagExp(&flagExp)
{
}

CustomScript::ObjectFlagTestExpression::ObjectFlagTestExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , objectExp(NULL)
    , flagExp(NULL)
{
    try
    {
        objectExp = &ObjectExpression::loadTyped(stream, script);
        flagExp = &StringExpression::loadTyped(stream, script);
    }
    catch (...)
    {
        delete objectExp;
        delete flagExp;
        throw;
    }
}

CustomScript::ObjectFlagTestExpression::~ObjectFlagTestExpression()
{
    delete objectExp;
    delete flagExp;
}

bool CustomScript::ObjectFlagTestExpression::evaluate()
{
    return objectExp->evaluate().testFlag(flagExp->evaluate());
}

BoolExpression * CustomScript::ObjectFlagTestExpression::TryParse(ParseData & data)
{
    ObjectExpression * objectExp = ObjectExpression::TryParse(data);

    if (!objectExp)
        return NULL;

    try
    {
        StringExpression * flagExp = IdentAsStringExpression::TryParse(data);    
        
        if (!flagExp)
        {
            delete objectExp;
            return NULL;
        }

        return new ObjectFlagTestExpression(data.script, *objectExp, *flagExp);
    }
    catch (...)
    {
        delete objectExp;
        throw;
    }
}

void CustomScript::ObjectFlagTestExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    objectExp->save(stream);
    flagExp->save(stream);
}

// GlobalFlagTestExpression

BoolExpression::Type CustomScript::GlobalFlagTestExpression::getType()
{
    return etGlobalFlagTest;
}

CustomScript::GlobalFlagTestExpression::GlobalFlagTestExpression(Script & script, StringExpression  & flagExp)
    : BoolExpression(script)
    , flagExp(&flagExp)
{
}

CustomScript::GlobalFlagTestExpression::GlobalFlagTestExpression(FileStream & stream, Script & script)
    : BoolExpression(script)
    , flagExp(&StringExpression::loadTyped(stream, script))
{
}

CustomScript::GlobalFlagTestExpression::~GlobalFlagTestExpression()
{
    delete flagExp;
}

bool CustomScript::GlobalFlagTestExpression::evaluate()
{
    return getAction().getAdventure().testFlag(flagExp->evaluate());
}

BoolExpression * CustomScript::GlobalFlagTestExpression::TryParse(ParseData & data)
{                              
    IdentAsStringExpression * flagExp = IdentAsStringExpression::TryParse(data);

    if (!flagExp)
        return NULL;

    return new GlobalFlagTestExpression(data.script, *flagExp);
}

void CustomScript::GlobalFlagTestExpression::save(FileStream & stream)
{
    BoolExpression::save(stream);
    flagExp->save(stream);
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

CustomScript::Statement::Statement(ParseData & data)
    : script(data.script)
    , parent(data.parent)
    , next(NULL)
{
}

Statement & CustomScript::Statement::parse(ParseData & data)
{
    Statement & result = *new Statement(data);
    try
    {
        if (data.bounds.pos == data.bounds.end)
            return result;
        Statement * next = &result;
        while (true)
        {
            if (data.bounds.pos == data.bounds.end)
                return result;
            bool found = false;
            for (TryParseFunc func : TryParseList)
            {
                size_t oldPos = data.bounds.pos;

                next->next = func(data);
                if (next->next)
                {
                    next = next->next;
                    found = true;
                    break;
                }
                else
                {
                    data.bounds.pos = oldPos;
                }
            }
            if (!found)
                return result;
        }
    }
    catch (...)
    {
        delete &result;
        throw;
    }
    data.parent = data.parent->hasParent() ? &data.parent->getParent() : NULL;
}

CustomScript::Statement::Statement(FileStream & stream, Script & script)
    : script(script)
    , next(loadTyped(stream, script, false))                   
{
}

Statement::~Statement()
{
    delete next;
}

void Statement::setNext(Statement * next)
{
    this->next = next;
}

bool CustomScript::Statement::hasParent()
{
    return parent;
}

ControlStatement & Statement::getParent()
{
    if (!parent)
        throw(EStatementHasNoParent);
    return *parent;
}

LoopStatement & CustomScript::Statement::getLoopParent(bool setExitFlag)
{
    ControlStatement & p = getParent();

    if (LoopStatement * loopStatement = dynamic_cast<LoopStatement*>(&p))
        return *loopStatement;
    
    if (setExitFlag)
        p.doExit();
    return p.getLoopParent();
}

const Command::Result & Statement::getParams() const
{
    return script.getParams();
}

CustomAdventureAction & Statement::getAction() const
{
    return script.getAction();
}

void Statement::execute()
{
    if (next)
        return next->execute();
}

void CustomScript::Statement::saveTyped(FileStream & stream, Statement * statement)
{
    stream.write(statement != NULL);
    if (statement != NULL)
    {
        stream.write(static_cast<byte>(statement->getType()));
        saveTyped(stream, statement->next);
        statement->save(stream);
    }
}

Statement * CustomScript::Statement::loadTyped(FileStream & stream, Script & script, bool required)
{    
    if (!stream.readBool())
    {
        if (required)
            throw(EBinaryDamaged);
        else
            return NULL;
    }
    
    switch (static_cast<Type>(stream.readByte()))
    {
    case stStatement:
        return new Statement(stream, script);              
    case stIf:
        return new IfStatement(stream, script);              
    case stSwitch:
        return new SwitchStatement(stream, script);           
    case stWhile:
        return new WhileStatement(stream, script);           
    case stDoWhile:
        return new RepeatUntilStatement(stream, script);         
    case stBreak:
        return new BreakStatement(stream, script);             
    case stContinue:
        return new ContinueStatement(stream, script);         
    case stSkip:
        return new SkipStatement(stream, script);             
    case stProcedure:
        return new ProcedureStatement(stream, script);           
    }
    throw(EBinaryDamaged);
}

Statement::Type Statement::getType()
{
    return stStatement;
}                  

void CustomScript::Statement::save(FileStream & stream)
{
    // nothing to save
}

// ControlStatement

bool CustomScript::ControlStatement::exitOccured()
{
    return exitFlag;
}

CustomScript::ControlStatement::ControlStatement(ParseData & data)
    : Statement(data)
{
    data.parent = this;
}

CustomScript::ControlStatement::ControlStatement(FileStream & stream, Script & script)
    : Statement(stream, script)
{
}

void CustomScript::ControlStatement::doExit()
{
    exitFlag = true;
}

void CustomScript::ControlStatement::preExecute()
{
    exitFlag = false;
}

void CustomScript::ControlStatement::execute()
{
    if (!exitFlag)
        Statement::execute();
}

// ControlStatement

bool ConditionalStatement::evaluateCondition()
{
    return condition->evaluate();
}

ConditionalStatement::ConditionalStatement(ParseData & data, BoolExpression & condition)
    : ControlStatement(data)
    , condition(&condition)
{
}

CustomScript::ConditionalStatement::ConditionalStatement(FileStream & stream, Script & script)
    : ControlStatement(stream, script)
    , condition(&BoolExpression::loadTyped(stream, script))
{
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

// IfStatement

IfStatement::IfStatement(ParseData & data, BoolExpression & condition, Statement & thenPart)
    : ConditionalStatement(data, condition)
    , thenPart(&thenPart)
    , elsePart(NULL)
{
}

CustomScript::IfStatement::IfStatement(FileStream & stream, Script & script)
    : ConditionalStatement(stream, script)
    , thenPart(NULL)
    , elsePart(NULL)
{
    try
    {
        thenPart = Statement::loadTyped(stream, script);
        elsePart = Statement::loadTyped(stream, script, false);
    }
    catch (...)
    {
        delete thenPart;
        delete elsePart;
        throw;
    }
}

IfStatement::~IfStatement()
{
    delete thenPart;
    delete elsePart;
}

void IfStatement::execute()
{
    preExecute();
    if (evaluateCondition())
        thenPart->execute();
    else if (elsePart)
        elsePart->execute();
    ControlStatement::execute();
}

Statement * IfStatement::TryParse(ParseData & data)
{
    static const std::string ifExp("if");
    static const std::string thenExp("then");
    static const std::string elseExp("else");
    static const std::string elseifExp("elseif");
    static const std::string endExp("end");

    if (!quick_check(data.bounds, ifExp))
        return NULL;

    data.bounds.advance(ifExp.size());

    bool valid = true;
    BoolExpression * condition = BoolExpression::TryParse(data);
    if (!condition)
        throw(ECompile, "Expected boolean expression after \"if\"", data);

    if (!quick_check(data.bounds, thenExp))
    {
        delete condition;
        throw(ECompile, "Expected \"then\" after boolean expression of \"if\"", data);
    }
    data.bounds.advance(thenExp.size());

    try
    {
        Statement & thenPart = Statement::parse(data);

        try
        {
            IfStatement & result = *new IfStatement(data, *condition, thenPart);
            valid = false;
            IfStatement * lastIf = &result;
            try
            {
                while (quick_check(data.bounds, elseifExp))
                {
                    data.bounds.advance(elseifExp.size());

                    BoolExpression * elseCondition = BoolExpression::TryParse(data);

                    if (!elseCondition)
                        throw(ECompile, "Expected boolean expression after \"elseif\"", data);

                    try
                    {
                        if (!quick_check(data.bounds, thenExp))
                        {
                            delete elseCondition;
                            throw(ECompile, "Expected \"then\" after boolean expression of \"elseif\"", data);
                        }
                        data.bounds.advance(thenExp.size());

                        Statement & elsePart = Statement::parse(data);
                        IfStatement * next = new IfStatement(data, *elseCondition, elsePart);
                        lastIf->elsePart = next;
                        lastIf = next;
                    }
                    catch (...)
                    {
                        delete elseCondition;
                        throw;
                    }
                }

                if (quick_check(data.bounds, elseExp))
                {
                    data.bounds.advance(elseExp.size());
                    lastIf->elsePart = &Statement::parse(data);
                }

                if (quick_check(data.bounds, endExp))
                {
                    data.bounds.advance(endExp.size());
                    return &result;
                }

                throw(ECompile, "Expected valid statement, \"elseif\" or \"end\"", data);
            }
            catch (...)
            {
                delete &result;
                throw;
            }
        }
        catch (...)
        {
            if (valid)
                delete &thenPart;
            throw;
        }
    }
    catch (...)
    {
        if (valid)
            delete condition;
        throw;
    }  
}

Statement::Type CustomScript::IfStatement::getType()
{
    return stIf;
}

void CustomScript::IfStatement::save(FileStream & stream)
{
    ConditionalStatement::save(stream);
    saveTyped(stream, thenPart);
    saveTyped(stream, elsePart);
}

// SwitchStatement

SwitchStatement::CaseSection::CaseSection(ObjectExpression & ident, Statement & statement)
    : ident(ident)
    , statement(statement)
{
}

SwitchStatement::SwitchStatement(ParseData & data, StringExpression & switchExp, std::vector<CaseSection> caseParts)
    : ControlStatement(data)
    , switchExp(&switchExp)
    , caseParts(caseParts)
    , elsePart(NULL)
{
}

CustomScript::SwitchStatement::SwitchStatement(FileStream & stream, Script & script)
    : ControlStatement(stream, script)
    , switchExp(NULL)
    , elsePart(NULL)
{
    try
    {
        switchExp = &StringExpression::loadTyped(stream, script);
        UINT length = stream.readUInt();
        for (UINT i = 0; i < length; i++)
        {
            ObjectExpression * identExp = NULL;
            Statement * stmt = NULL;
            try
            {
                identExp = &ObjectExpression::loadTyped(stream, script);
                stmt = Statement::loadTyped(stream, script);
                caseParts.push_back(CaseSection(*identExp, *stmt));
            }
            catch (...)
            {
                delete &identExp;
                delete &stmt;
                throw;
            }
        }
        elsePart = Statement::loadTyped(stream, script);
    }
    catch (...)
    {
        delete switchExp;
        delete elsePart;
        throw;
    }
}

SwitchStatement::~SwitchStatement()
{
    delete switchExp;
    delete elsePart;
    for (CaseSection & section : caseParts)
    {
        delete &section.ident;
        delete &section.statement;
    }
}

void SwitchStatement::execute()
{
    preExecute();
    bool found = false;
    try
    {
        AdventureObject & object = getAction().getAdventure().findObjectByAlias(switchExp->evaluate());
        for (const CaseSection & section : caseParts)
        {
            if (&object == &section.ident.evaluate())
            {
                section.statement.execute();
                found = true;
                break;
            }
        }
    }
    catch (EAdventureObjectAliasNotFound) {}

    if (!found && elsePart)
        return elsePart->execute();

    ControlStatement::execute();
}

Statement * SwitchStatement::TryParse(ParseData & data)
{
    static const std::string caseExp("case");
    static const std::string ofExp("of");
    static const std::string labelExp(":");
    static const std::string elseExp("else");
    static const std::string endExp("end");

    if (!quick_check(data.bounds, caseExp))
        return NULL;
    data.bounds.advance(caseExp.size());

    StringExpression * switchExp = ParamExpression::TryParse(data);
   
    if (!switchExp)
        throw(ECompile, "Expected parameter expression", data);

    if (!quick_check(data.bounds, ofExp))
        throw(ECompile, "Expected \"of\" after parameter expression of \"case\"", data);
    
    data.bounds.advance(ofExp.size());

    std::vector<CaseSection> caseParts;

    while (quick_check(data.bounds, labelExp))
    {
        data.bounds.advance(labelExp.size());    

        ObjectExpression * expr = IdentExpression::TryParse(data); 
            
        if (!expr)
        {
            delete switchExp;
            throw(ECompile, "Expected case label", data);
        }
            
        try
        {
            Statement & caseStmt = Statement::parse(data);
            caseParts.push_back(CaseSection(*expr, caseStmt));
        }
        catch (...)
        {
            delete &expr;
            throw;
        }                          

    }

    SwitchStatement * result = new SwitchStatement(data, *switchExp, caseParts);

    if (quick_check(data.bounds, elseExp))
    {
        data.bounds.advance(elseExp.size());
        result->setElsePart(Statement::parse(data));
    }

    if (!quick_check(data.bounds, endExp))
    {
        delete result;
        throw(ECompile, "Expected valid statement or \"end\"", data);
    }

    data.bounds.advance(endExp.size());

    return result;

}

void CustomScript::SwitchStatement::setElsePart(Statement & elsePart)
{
    this->elsePart = &elsePart;
}

Statement::Type CustomScript::SwitchStatement::getType()
{
    return stSwitch;
}

void CustomScript::SwitchStatement::save(FileStream & stream)
{
    ControlStatement::save(stream);
    switchExp->save(stream);
    stream.write(static_cast<UINT>(caseParts.size()));
    for (const CaseSection & caseSection : caseParts)
    {
        caseSection.ident.save(stream);
        saveTyped(stream, &caseSection.statement);
    }
    saveTyped(stream, elsePart);
}

// LoopStatement

void LoopStatement::executeLoopPart()
{
    if (loopPart)
        loopPart->execute();
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

LoopStatement::LoopStatement(ParseData & data, BoolExpression & condition, Statement & loopPart)
    : ConditionalStatement(data, condition)
    , loopPart(&loopPart)
{
}

CustomScript::LoopStatement::LoopStatement(FileStream & stream, Script & script)
    : ConditionalStatement(stream, script)
    , loopPart(Statement::loadTyped(stream, script))
{
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

// WhileStatement

CustomScript::WhileStatement::WhileStatement(ParseData & data, BoolExpression & condition, Statement & loopPart)
    : LoopStatement(data, condition, loopPart)
{
}

CustomScript::WhileStatement::WhileStatement(FileStream & stream, Script & script)
    : LoopStatement(stream, script)
{
}

void WhileStatement::execute()
{
    preExecute();
    while (!breakOccured() && evaluateCondition())
        executeLoopPart();
    ConditionalStatement::execute();
}

Statement * WhileStatement::TryParse(ParseData & data)
{
    static const std::string whileExp("while");
    static const std::string doExp("do");
    static const std::string endExp("end");

    if (!quick_check(data.bounds, whileExp))
        return NULL;

    data.bounds.advance(whileExp.size());

    BoolExpression * condition = BoolExpression::TryParse(data);

    if (!condition)
        throw(ECompile, "Expected boolean expression after \"while\"", data);

    try
    {
        if (!quick_check(data.bounds, doExp))
            throw(ECompile, "Expected \"do\" after boolean expression of \"while\"", data);

        data.bounds.advance(doExp.size());

        Statement & loopPart = Statement::parse(data);

        if (!quick_check(data.bounds, endExp))
        {
            delete &loopPart;
            throw(ECompile, "Expected valid statement or \"end\"", data);
        }
        data.bounds.advance(endExp.size());

        return new WhileStatement(data, *condition, loopPart);
    }
    catch (...) 
    {
        delete condition;
        throw;
    }
}

Statement::Type CustomScript::WhileStatement::getType()
{
    return stWhile;
}

// RepeatUntilStatement

CustomScript::RepeatUntilStatement::RepeatUntilStatement(ParseData & data, BoolExpression & condition, Statement & loopPart)
    : LoopStatement(data, condition, loopPart)
{
}

CustomScript::RepeatUntilStatement::RepeatUntilStatement(FileStream & stream, Script & script)
    : LoopStatement(stream, script)
{
}

void RepeatUntilStatement::execute()
{
    preExecute();
    do
    {
        executeLoopPart();
    } while (!breakOccured() && !evaluateCondition());
    ConditionalStatement::execute();
}

Statement * RepeatUntilStatement::TryParse(ParseData & data)
{
    static const std::string repeatExp("repeat");
    static const std::string untilExp("until");

    if (!quick_check(data.bounds, repeatExp))
        return NULL;

    data.bounds.advance(repeatExp.size());

    Statement & loopPart = Statement::parse(data);

    try
    {
        if (!quick_check(data.bounds, untilExp))
            throw(ECompile, "Expected \"until\"", data);
        data.bounds.advance(untilExp.size());

        BoolExpression * condition = BoolExpression::TryParse(data);

        if (!condition)
        {
            throw(ECompile, "Expected boolean expression for \"until\"", data);
        }

        return new RepeatUntilStatement(data, *condition, loopPart);

    }
    catch (...)
    {
        delete &loopPart;
        throw;
    }
}

Statement::Type CustomScript::RepeatUntilStatement::getType()
{
    return stDoWhile;
}

// BreakStatement

CustomScript::BreakStatement::BreakStatement(ParseData & data)
    : Statement(data)
{
    try
    {
        getLoopParent();
    }
    catch (EStatementHasNoParent)
    {
        throw(ECompile, "\"break\" must be inside of a loop", data);
    }
}

CustomScript::BreakStatement::BreakStatement(FileStream & stream, Script & script)
    : Statement(stream, script)
{
}

void BreakStatement::execute()
{
    getLoopParent(true).doBreak();
}

Statement * BreakStatement::TryParse(ParseData & data)
{
    static const std::string breakExp("break");

    if (!quick_check(data.bounds, breakExp))
        return NULL;

    data.bounds.advance(breakExp.size());
    
    return new BreakStatement(data);
}

Statement::Type CustomScript::BreakStatement::getType()
{
    return stBreak;
}

// ContinueStatement

CustomScript::ContinueStatement::ContinueStatement(ParseData & data)
    : Statement(data)
{
    try
    {
        getLoopParent();
    }
    catch (EStatementHasNoParent)
    {
        throw(ECompile, "\"continue\" must be inside of a loop", data);
    }
}

CustomScript::ContinueStatement::ContinueStatement(FileStream & stream, Script & script)
    : Statement(stream, script)
{
}

void ContinueStatement::execute()
{
    getLoopParent(true).doContinue();
}

Statement * ContinueStatement::TryParse(ParseData & data)
{
    static const std::string continueExp("continue");

    if (!quick_check(data.bounds, continueExp))
        return NULL;

    data.bounds.advance(continueExp.size());

    return new ContinueStatement(data);
}

Statement::Type CustomScript::ContinueStatement::getType()
{
    return stContinue;
}

// SkipStatement

CustomScript::SkipStatement::SkipStatement(ParseData & data)
    : Statement(data)
{
}

CustomScript::SkipStatement::SkipStatement(FileStream & stream, Script & script)
    : Statement(stream, script)
{
}

void SkipStatement::execute()
{
    throw(ESkip);
}

Statement * SkipStatement::TryParse(ParseData & data)
{
    static const std::string skipExp("skip");

    if (!quick_check(data.bounds, skipExp))
        return NULL;

    data.bounds.advance(skipExp.size());

    return new SkipStatement(data);
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

CustomScript::ProcedureStatement::ProcedureStatement(ParseData & data, ProcedureType type)
    : Statement(data)
    , type(type)
{
}

CustomScript::ProcedureStatement::ProcedureStatement(FileStream & stream, Script & script)
    : Statement(stream, script)
    , type(static_cast<ProcedureType>(stream.readByte()))
{
    for (ExpressionType expressionType : Functions[type].params)  
        params.push_back(&Expression::loadTyped(stream, expressionType, script));
}

ProcedureStatement::~ProcedureStatement()
{
    for (Expression * expr : params)
        delete expr;
}

void ProcedureStatement::execute()
{
    auto getObject = [&](int param) -> AdventureObject&
    {
        return static_cast<ObjectExpression&>(*params[param]).evaluate();
    };

    auto getRoom = [&](int param) -> Room&
    {
        AdventureObject & object = getObject(param);
        try
        {
            return dynamic_cast<Room&>(object);
        }
        catch (std::bad_cast)
        {
            throw(ERoomTypeConflict, object);
        }
    };

    auto getItem = [&](int param) -> Item&
    {
        AdventureObject & object = getObject(param);
        try
        {
            return dynamic_cast<Item&>(object);
        }
        catch (std::bad_cast)
        {
            throw(EItemTypeConflict, object);
        }
    };

    auto getLocation = [&](int param) -> Location&
    {
        AdventureObject & object = getObject(param);
        try
        {
            return dynamic_cast<Location&>(object);
        }
        catch (std::bad_cast)
        {
            throw(ELocationTypeConflict, object);
        }
    };

    auto getRoomConnection = [&](int param) -> RoomConnection&
    {
        AdventureObject & object = getObject(param);
        try
        {
            return dynamic_cast<RoomConnection&>(object);
        }
        catch (std::bad_cast)
        {
            throw(ERoomConnectionTypeConflict, object);
        }
    };

    auto getString = [&](int param)
    {
        return static_cast<StringExpression*>(params[param])->evaluate();
    };

    switch (type)
    {
    case ptWrite: {
        getAction().write(getString(0));
        break;
    }
    case ptDraw: {
        throw(ENotImplemented, "AdventureScript Draw-Procedure");
        break;
    }            

    case ptSetRoom: {
        getAction().getPlayer().gotoRoom(getRoom(0));
        break;
    }
    case ptSetLocation: {
        Location & location = getLocation(0);
        if (!getAction().currentRoom().hasLocation(location))
            throw(ELocationMissing, location, getAction().currentRoom());
        getAction().getPlayer().gotoLocation(location);
        break;
    }                
    case ptPlayerAddItem: {
        getAction().getPlayerInv().addItem(getItem(0));
        break;
    }
    case ptPlayerDelItem: {
        getAction().getPlayerInv().delItem(getItem(0));
        break;
    }
    case ptLocationAddItem: {
        getLocation(2).getInventory(getString(1)).addItemForce(getItem(0));
        break;
    }
    case ptLocationDelItem: {        
        getLocation(2).getInventory(getString(1)).delItem(getItem(0));
        break;
    }

    case ptFilterAdd: {
        getLocation(2).getInventory(getString(1)).addToFilter(getItem(0));
        break;
    }
    case ptFilterDel: {
        getLocation(2).getInventory(getString(1)).delFromFilter(getItem(0));
        break;
    }
    case ptFilterWhitelist: {
        getLocation(1).getInventory(getString(0)).setFilterMode(Location::MultiInventory::ifWhitelist);
        break;
    }
    case ptFilterBlacklist: {
        getLocation(1).getInventory(getString(0)).setFilterMode(Location::MultiInventory::ifBlacklist);
        break;
    }
    case ptFilterDisable: {
        getLocation(1).getInventory(getString(0)).filterAllowAll();
        break;
    }

    case ptSetDescription: {
        getObject(0).setDescription(getString(1));
        break;
    }
    case ptAddAlias: {
        getObject(0).getAliases().add(getString(1));
        break;
    }
    case ptDelAlias: {
        getObject(0).getAliases().del(getString(1));
        break;
    }

    case ptPlayerInform: {
        getAction().getPlayer().inform(getObject(0));
        break;
    }
    case ptPlayerForget: {
        getAction().getPlayer().forget(getObject(0));
        break;
    }

    case ptLock: {
        getRoomConnection(0).lock();
        break;
    }
    case ptUnlock: {
        getRoomConnection(0).unlock();
        break;
    }

    case ptAddItemCombination: {
        getAction().getItemCombiner().addCombination(getItem(0), getItem(1), getItem(2));
        break;
    }
    case ptDelItemCombination: {
        getAction().getItemCombiner().delCombination(getItem(0), getItem(1)); 
        break;
    }

    case ptSet: {
        getObject(0).setFlag(getString(1));
        break;
    }
    case ptClear: {
        getObject(0).clearFlag(getString(1));
        break;
    }       
    case ptToggle: {
        getObject(0).toggleFlag(getString(1));
        break;
    }
    case ptGlobalSet: {
        getAction().getAdventure().setFlag(getString(0));
        break;
    }
    case ptGlobalClear: {
        getAction().getAdventure().clearFlag(getString(0));
        break;
    }
    case ptGlobalToggle: {
        getAction().getAdventure().toggleFlag(getString(0));
        break;
    }
                
    case ptRunWith: {
        AdventureObject & object = getObject(0);
        std::string cmd = getString(1);

        try
        {
            dynamic_cast<Room&>(object).getLocatedCommands().sendCommand(cmd);
            break;
        }
        catch (std::bad_cast) {}

        try
        {
            dynamic_cast<Location&>(object).getLocatedCommands().sendCommand(cmd);
            break;
        }
        catch (std::bad_cast) {}

        try
        {
            dynamic_cast<Item&>(object).getCarryCommands().sendCommand(cmd);
            break;
        }
        catch (std::bad_cast) {}
        
        throw(ERunWithUnknownObjectType, object);
        break;
    }
    }
    return Statement::execute();
}

Statement * ProcedureStatement::TryParse(ParseData & data)
{
    std::string ident;
    
    if (!parse_ident(data.bounds, ident))
        return NULL;

    bool found = false;
    ProcedureType funcType;
    for (funcType = ptWrite; funcType < PROCEDURE_COUNT; funcType = static_cast<ProcedureType>(funcType + 1))
    {
        if (Functions[funcType].name == ident)
        {
            found = true;
            break;
        }
    }
    if (!found)
        return NULL;

    data.bounds.advance(ident.size());

    ProcedureStatement & result = *new ProcedureStatement(data, funcType);

    try
    {
        for (ExpressionType exprType : Functions[result.type].params)
        {
            switch (exprType)
            {
            case etObject:
                if (auto param = ObjectExpression::TryParse(data))
                {
                    result.params.push_back(param);
                }
                else
                {
                    throw(ECompile, "Expected object expression for \"" + Functions[result.type].name + "\"", data);
                }   
                break;
            case etBool:
                if (auto param = BoolExpression::TryParse(data))
                {
                    result.params.push_back(param);
                }
                else
                {
                    throw(ECompile, "Expected boolean expression for \"" + Functions[result.type].name + "\"", data);
                }
                break;
            case etString:
                if (auto param = StringExpression::TryParse(data))
                {
                    result.params.push_back(param);
                }
                else
                {
                    throw(ECompile, "Expected string expression for \"" + Functions[result.type].name + "\"", data);
                }
                break;
            case etIdent:
                if (auto param = IdentAsStringExpression::TryParse(data))
                {
                    result.params.push_back(param);
                }
                else
                {
                    throw(ECompile, "Expected identifier for \"" + Functions[result.type].name + "\"", data);
                }
                break;
            }
        }    

        return &result;
    }
    catch (...)
    {
        delete &result;
        throw;
    }
}

Statement::Type CustomScript::ProcedureStatement::getType()
{
    return stProcedure;
}

void CustomScript::ProcedureStatement::save(FileStream & stream)
{
    Statement::save(stream);
    stream.write(static_cast<byte>(type));
    for (Expression * param : params)
        param->save(stream);
}

// Script

CustomScript::Script::Script(CustomAdventureAction & action, FileStream & stream)
    : title(stream.readString())
    , action(action)
    , requiredParams(stream.readTags())
    , root(*Statement::loadTyped(stream, *this))
{                 
}

Script::Script(CustomAdventureAction & action, std::string code, std::string title, const AdventureStructure::LineInfo & lineinfo)
try : code(new std::string(code))
    , lineinfo(lineinfo)
    , parseData(new ParseData(StringBounds(*this->code), *this, NULL))
    , title(title)
    , action(action)
    , root(Statement::parse(*parseData))
{
    if (parseData->bounds.pos != parseData->bounds.end)
        throw(ECompile, "Unknown procedure", *parseData);

    delete this->code;
    this->code = NULL;
    delete parseData;
    parseData = NULL;
}
catch (...)
{
    delete this->code;
    delete parseData;
    throw;
}

CustomScript::Script::~Script()
{
    delete &root;
}

bool Script::run(const Command::Result & params)
{
    this->params = &params;
    try
    {
        root.execute();
        return true;
    }
    catch (ESkip)
    {
        return false;
    }
}

const Command::Result & Script::getParams() const
{
    return *params;
}

CustomAdventureAction & Script::getAction() const
{
    return action;
}

taglist & CustomScript::Script::getRequiredParams()
{
    return requiredParams;
}

const taglist & CustomScript::Script::getRequiredParamsConst() const
{
    return requiredParams;
}

const std::string & CustomScript::Script::getCode()
{
    return *code;
}

const AdventureStructure::LineInfo & CustomScript::Script::getLineInfo() const
{
    return lineinfo;
}

bool Script::succeeded() const
{
    return success;
}

void CustomScript::Script::save(FileStream & stream) const
{
    stream.write(title);
    stream.write(requiredParams);
    Statement::saveTyped(stream, &root);
}

// Global

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

CustomScript::EScript::EScript(const std::string & msg)
    : Exception(msg)
{
}

CustomScript::ECompile::ECompile(const std::string & msg, const ParseData & data)
    : EScript(msg)
{
    lineinfo.line = data.script.getLineInfo().line;
    ptrdiff_t codeLines = std::count(data.bounds.text.begin(), data.bounds.text.begin() + data.bounds.pos, '\n');
    if (codeLines == 0)
    {
        lineinfo.col = data.script.getLineInfo().col;
    }
    else
    {
        lineinfo.line += codeLines;
        lineinfo.col = data.bounds.pos - data.bounds.text.find_last_of('\n', data.bounds.pos);
    }
}

const AdventureStructure::LineInfo & CustomScript::ECompile::getErrorLine() const
{
    return lineinfo;
}

CustomScript::ERuntime::ERuntime(const std::string & msg)
    : EScript("Script Runtime Error: " + msg)
{
}

CustomScript::EObjectEvaluation::EObjectEvaluation(const std::string & identifier)
    : ERuntime("Could not evaluate \"" + identifier + "\"")
{
}

CustomScript::EUnknownParam::EUnknownParam(const std::string & param)
    : ERuntime("Unknown parameter \"" + param + "\"")
{
}

CustomScript::EObjectTypeConflict::EObjectTypeConflict(const AdventureObject & object, const std::string & expectedType)
    : ERuntime("Object typed conflict: " + object.getName() + " should be " + expectedType)
{
}

CustomScript::EItemTypeConflict::EItemTypeConflict(const AdventureObject & object)
    : EObjectTypeConflict(object, "item")
{
}

CustomScript::ERoomTypeConflict::ERoomTypeConflict(const AdventureObject & object)
    : EObjectTypeConflict(object, "room")
{
}

CustomScript::ELocationTypeConflict::ELocationTypeConflict(const AdventureObject & object)
    : EObjectTypeConflict(object, "location")
{
}

CustomScript::EPrepositionMissing::EPrepositionMissing(const Location & location, const std::string & preposition)
    : ERuntime("Location \"" + location.getName() + "\" is missing preposition \"" + preposition + "\"")
{
}

CustomScript::ELocationMissing::ELocationMissing(const Location & location, const Room & room)
    : ERuntime("Location \"" + location.getName() + "\" missing in room \"" + room.getName() + "\"")
{
}

CustomScript::ERunWithUnknownObjectType::ERunWithUnknownObjectType(AdventureObject & object)
    : Exception("Object \"" + object.getName() + "\" does not have CustomCommands and can therefore not be used with run_with")
{
}

CustomScript::EStatementHasNoParent::EStatementHasNoParent()
    : EScript("Statement does not have a parent")
{
}

CustomScript::ESkip::ESkip()
    : ERuntime("Code skipped")
{
}

CustomScript::ERoomConnectionTypeConflict::ERoomConnectionTypeConflict(const AdventureObject & object)
    : EObjectTypeConflict(object, "room connection")
{
}
