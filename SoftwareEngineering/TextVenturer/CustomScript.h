#pragma once

#include "Command.h"

class Adventure;
class AdventureObject;
class Location;
class Room;
class CustomAdventureAction;

namespace CustomScript
{
    class ControlStatement;
    class LoopStatement;
    class Script;

    struct StringBounds
    {
        const std::string & text;
        size_t pos;
        size_t end;
        void advance(size_t amount, bool seekNext = true);

        StringBounds(const std::string & text, size_t pos = 0, size_t end = std::string::npos);
    };                                      

    struct ParseData
    {
        StringBounds bounds;
        ref_optional<ControlStatement> parent;
        Script & script;
        bool skipLogicOp;

        ParseData(StringBounds bounds, Script & script, ControlStatement & parent);
        ParseData(StringBounds bounds, Script & script);
    };

    class ParamExpression;

    enum ExpressionType {
        etObject,
        etBool,
        etString,
        etIdent,
        EXPRESSION_TYPE_COUNT
    };

    // --- Expressions ---
    class Expression abstract
    {
    private:
        Script & script;
    public:
        Expression(Script & script) : script(script) {}
        virtual ~Expression() {}
        Script & getScript() const;
        const Command::Result & getParams() const;
        CustomAdventureAction & getAction() const;

        static Expression * loadTyped(FileStream & stream, ExpressionType type, Script & script);
        virtual void save(FileStream & stream) = 0;
    };

    template <typename ResultType>
    class TypedExpression abstract : public Expression
    {
    public:
        TypedExpression(Script & script) : Expression(script) {}
        virtual ResultType evaluate() = 0;
        virtual ExpressionType getResultType() = 0;
    };

    // Evaluation Types
    class ObjectExpression abstract : public TypedExpression<AdventureObject&>
    {
    protected:
        enum Type
        {
            etIdent
        };

        virtual Type getType() = 0;

    public:
        ObjectExpression(Script & script) : TypedExpression<AdventureObject&>(script) {}
        static ObjectExpression * TryParse(ParseData & data);
        ExpressionType getResultType();

        typedef ObjectExpression * (*TryParseFunc)(ParseData&);
        static const TryParseFunc TryParseList[];

        static ObjectExpression * loadTyped(FileStream & stream, Script & script);
        void save(FileStream & stream);
    };

    class StringExpression abstract : public TypedExpression<std::string>
    {
    protected:
        enum Type
        {
            etObjectToString,
            etConstString,
            etStringConcat,
            etParam,
            etIdentAsString
        };
        
        virtual Type getType() = 0;

    public:
        StringExpression(Script & script) : TypedExpression<std::string>(script) {}
        static StringExpression * TryParse(ParseData &data);
        ExpressionType getResultType();

        typedef StringExpression * (*TryParseFunc)(ParseData&);
        static const TryParseFunc TryParseList[];

        static StringExpression * loadTyped(FileStream & stream, Script & script);
        void save(FileStream & stream);
    };

    class BoolExpression abstract : public TypedExpression<bool>
    {
    protected:
        enum Type
        {
            etParamIsIdent,
            etConstBool,
            etPlayerHasItem,
            etBracket,
            etLogicNot,
            etLogicOp,
            etLocationHasItem,
            etObjectFlagTest,
            etGlobalFlagTest
        };

        virtual Type getType() = 0;

    public:
        BoolExpression(Script & script) : TypedExpression<bool>(script) {}
        static BoolExpression * TryParse(ParseData &data);
        ExpressionType getResultType();

        typedef BoolExpression * (*TryParseFunc)(ParseData&);
        static const TryParseFunc TryParseList[];

        static BoolExpression * loadTyped(FileStream & stream, Script & script);
        void save(FileStream & stream);
    };

    // Evaluation > Object
    class IdentExpression : public ObjectExpression
    {
    private:
        std::string identifier;

    protected:                 
        Type getType();

    public:
        IdentExpression(Script & script) : ObjectExpression(script) {}
        IdentExpression(FileStream & stream, Script & script);
        AdventureObject& evaluate();

        static ObjectExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    // Evalutation > String
    class ObjectToStringExpression : public StringExpression
    {
    public:
        enum GenerateType
        {
            gtArticleFromPlayer,
            gtDefiniteArticle,
            gtIndefiniteAricle,
            gtNameOnly
        };
    private:
        ObjectExpression * objectExp;
        bool startOfSentence;
        GenerateType type;

    protected:
        Type getType();

    public:
        ObjectToStringExpression(Script & script, ObjectExpression * objectExp, bool startOfSentence, GenerateType type);
        ObjectToStringExpression(FileStream & stream, Script & script);
        ~ObjectToStringExpression();
        std::string evaluate();

        static StringExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class ConstStringExpression : public StringExpression
    {
    private:
        std::string text;

    protected:
        Type getType();

    public:
        ConstStringExpression(Script & script, std::string text);
        ConstStringExpression(FileStream & stream, Script & script);
        std::string evaluate();

        static StringExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class StringConcatExpression : public StringExpression
    {
    private:
        std::vector<StringExpression*> stringExpList;

    protected:
        Type getType();

    public:
        StringConcatExpression(Script & script, std::vector<StringExpression*> stringExpList);
        StringConcatExpression(FileStream & stream, Script & script);
        ~StringConcatExpression();
        std::string evaluate();
                                        
        static StringConcatExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class ParamExpression : public StringExpression
    {
    private:
        std::string param;

    protected:
        Type getType();

    public:
        ParamExpression(Script & script, std::string param);
        ParamExpression(FileStream & stream, Script & script);
        std::string evaluate();

        static StringExpression * TryParse(ParseData &data);

        void save(FileStream & stream);
    };

    class IdentAsStringExpression : public StringExpression
    {
    private:
        std::string identString;

    protected:
        Type getType();

    public:
        IdentAsStringExpression(Script & script, std::string identString);
        IdentAsStringExpression(FileStream & stream, Script & script);
        std::string evaluate();

        static IdentAsStringExpression * TryParse(ParseData &data);

        void save(FileStream & stream);
    };

    // Evaluation > Bool  
    class ParamIsIdentExpression : public BoolExpression
    {
    private:
        ParamExpression * paramExp;
        IdentExpression * identExp;

    protected:
        Type getType();

    public:
        ParamIsIdentExpression(Script & script, ParamExpression * paramExp, IdentExpression * identExp);
        ParamIsIdentExpression(FileStream & stream, Script & script);
        ~ParamIsIdentExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);
        
        void save(FileStream & stream);
    };

    class ConstBoolExpression : public BoolExpression
    {
    private:
        bool value;

    protected:
        Type getType();

    public:
        ConstBoolExpression(Script & script, bool value);
        ConstBoolExpression(FileStream & stream, Script & script);
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class PlayerHasItemExpression : public BoolExpression
    {
    private:
        ObjectExpression * itemExp;

    protected:
        Type getType();

    public:
        PlayerHasItemExpression(Script & script, ObjectExpression * itemExp);
        PlayerHasItemExpression(FileStream & stream, Script & script);
        ~PlayerHasItemExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class BracketExpression : public BoolExpression
    {
    private:
        BoolExpression * boolExp;

    protected:
        Type getType();

    public:
        BracketExpression(Script & script, BoolExpression * boolExp);
        BracketExpression(FileStream & stream, Script & script);
        ~BracketExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class LogicNotExpression : public BoolExpression
    {
    private:
        BoolExpression * boolExp;

    protected:
        Type getType();

    public:
        LogicNotExpression(Script & script, BoolExpression * boolExp);
        LogicNotExpression(FileStream & stream, Script & script);
        ~LogicNotExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class LogicOpExpression : public BoolExpression
    {
    public:
        enum LogicalOperation
        {
            opAND,
            opOR,
            opXOR
        };
    private:
        BoolExpression * boolExp1;
        BoolExpression * boolExp2;
        LogicalOperation operation;

    protected:
        Type getType();

    public:
        LogicOpExpression(Script & script, BoolExpression * boolExp1, BoolExpression * boolExp2, LogicalOperation operation);
        LogicOpExpression(FileStream & stream, Script & script);
        ~LogicOpExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class LocationHasItemExpression : public BoolExpression
    {
    private:
        ObjectExpression * locationExp;
        ObjectExpression * itemExp;
        IdentAsStringExpression * prepositionExp;

    protected:
        Type getType();

    public:
        LocationHasItemExpression(Script & script, ObjectExpression * locationExp, ObjectExpression * itemExp, IdentAsStringExpression * prepositionExp);
        LocationHasItemExpression(FileStream & stream, Script & script);
        ~LocationHasItemExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class ObjectFlagTestExpression : public BoolExpression
    {
    private:
        ObjectExpression * objectExp;
        IdentAsStringExpression * flagExp;

    protected:
        Type getType();

    public:
        ObjectFlagTestExpression(Script & script, ObjectExpression * objectExp, IdentAsStringExpression * flagExp);
        ObjectFlagTestExpression(FileStream & stream, Script & script);
        ~ObjectFlagTestExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    class GlobalFlagTestExpression : public BoolExpression
    {
    private:
        IdentAsStringExpression * flagExp;

    protected:
        Type getType();

    public:
        GlobalFlagTestExpression(Script & script, IdentAsStringExpression * flagExp);
        GlobalFlagTestExpression(FileStream & stream, Script & script);
        ~GlobalFlagTestExpression();
        bool evaluate();

        static BoolExpression * TryParse(ParseData & data);

        void save(FileStream & stream);
    };

    // --- Statements ---          
    class Statement
    {
    public:
        enum ParseResult
        {
            prError,
            prUnknownCommand,
            prSuccess
        }; 

        enum Type
        {
            stStatement,
            stIf,
            stSwitch,
            stWhile,
            stDoWhile,
            stBreak,
            stContinue,
            stSkip,
            stProcedure
        };

    private:
        Statement * next;
        ref_optional<ControlStatement> parent;
        Script & script;
                 
    public:
        Statement(Script & script);
        Statement(Script & script, ControlStatement & parent);
        Statement(FileStream & stream, Script & script);
        virtual ~Statement();
        void setNext(Statement* next);
        bool hasParent();
        ControlStatement & getParent();
        LoopStatement & getLoopParent(bool setExitFlag = false);
        const Command::Result & getParams() const;
        CustomAdventureAction & getAction() const;
        virtual bool execute();

        ParseResult parse(ParseData & data);

        typedef Statement * (*TryParseFunc)(ParseData&);
        static const TryParseFunc TryParseList[];     

        static Statement * loadTyped(FileStream & stream, Script & script);

        virtual Type getType();
        virtual void save(FileStream & stream);
    };

    class ControlStatement abstract : public Statement
    {
    private:
        bool exitFlag;

    protected:
        bool exitOccured();

    public:
        void doExit();
        virtual void preExecute();
        bool execute();
    };

    class ConditionalStatement abstract : public ControlStatement
    {
    protected:
        BoolExpression & condition;
        bool evaluateCondition();

    public:
        ConditionalStatement();
        ConditionalStatement(FileStream & stream, Script & script);
        ~ConditionalStatement();

        void save(FileStream & stream);
    };

    class IfStatement : public ConditionalStatement
    {
    private:
        Statement & thenPart;
        Statement & elsePart;
    
    public:
        IfStatement();
        IfStatement(FileStream & stream, Script & script);
        ~IfStatement();
        bool execute();
        static Statement * TryParse(ParseData & data);

        Type getType();
        void save(FileStream & stream);
    };

    class SwitchStatement : public ControlStatement
    {
    public:
        struct CaseSection
        {
            IdentExpression & ident;
            Statement & statement;
            CaseSection(IdentExpression & ident, Statement & statement);
        };

    private:
        ParamExpression & switchExpr;
        std::vector<CaseSection> caseParts;
        Statement & elsePart;
    
    public:
        SwitchStatement();
        SwitchStatement(FileStream & stream, Script & script);
        ~SwitchStatement();
        bool execute();
        static Statement * TryParse(ParseData& data);

        Type getType();
        void save(FileStream & stream);
    };

    class LoopStatement abstract : public ConditionalStatement
    {
    private:
        bool breakFlag;
        bool continueFlag;
        Statement & loopPart;

    protected:
        bool executeLoopPart();
        bool breakOccured();
        bool continueOccured();
        void preExecute();

    public:
        LoopStatement(Statement & loopPart);
        LoopStatement(FileStream & stream, Script & script);
        ~LoopStatement();
        void doBreak();
        void doContinue();

        void save(FileStream & stream);
    };

    class WhileStatement : public LoopStatement
    {
    public:
        bool execute();
        static Statement * TryParse(ParseData & data);
        
        Type getType();
    };

    class RepeatUntilStatement : public LoopStatement
    {
    public:
        bool execute();
        static Statement * TryParse(ParseData & data);
        
        Type getType();    
    };

    class BreakStatement : public Statement
    {
    public:
        bool execute();
        static Statement * TryParse(ParseData & data);
        
        Type getType();
    };

    class ContinueStatement : public Statement
    {
    public:
        bool execute();
        static Statement * TryParse(ParseData & data);
        
        Type getType();
    };

    class SkipStatement : public Statement
    {
    public:
        bool execute();
        static Statement * TryParse(ParseData & data);
        
        Type getType();
    };

    class ProcedureStatement : public Statement
    {
    public:
        enum ProcedureType
        {
            ptWrite,
            ptDraw,

            ptSetRoom,
            ptSetLocation,

            ptPlayerAddItem,
            ptPlayerDelItem,
            ptLocationAddItem,
            ptLocationDelItem,

            ptFilterAdd,
            ptFilterDel,
            ptFilterWhitelist,
            ptFilterBlacklist,
            ptFilterDisable,

            ptSetDescription,
            ptAddAlias,
            ptDelAlias,

            ptPlayerInform,
            ptPlayerForget,

            ptLock,
            ptUnlock,

            ptAddItemCombination,
            ptDelItemCombination,

            ptSet,
            ptClear,
            ptToggle,
            ptGlobalSet,
            ptGlobalClear,
            ptGlobalToggle,

            ptRunWith,

            PROCEDURE_COUNT
        };

        struct ProcedureData
        {
            std::string name;
            std::vector<ExpressionType> params;
            ProcedureData(std::string name, std::vector<ExpressionType> params);
        };

        static const ProcedureData Functions[PROCEDURE_COUNT];
    private:
        ProcedureType type;
        std::vector<Expression*> params;
    
    public:
        ProcedureStatement();
        ProcedureStatement(FileStream & stream, Script & script);
        ~ProcedureStatement();
        bool execute();
        static Statement * TryParse(ParseData & data);

        Type getType();
        void save(FileStream & stream);
    };

    class Script
    {
    private:
        CustomAdventureAction & action;
        Statement & root;
        ref_optional<const Command::Result> params;
        std::string title;
        std::optional<std::string> code; 
        std::optional<ParseData> parseData;
        bool success;

        taglist requiredParams;

    public:
        Script(CustomAdventureAction & action, FileStream & stream);
        Script(CustomAdventureAction & action, std::string code, std::string title);
        ~Script();
        bool run(const Command::Result & params);
        const Command::Result & getParams() const;
        CustomAdventureAction & getAction() const;

        taglist & getRequiredParams() const;

        const std::string & getCode();

        bool succeeded() const;
        void error(std::string message) const;

        void save(FileStream & stream) const;
    };

    // bool check_regex(StringBounds bounds, std::smatch & matches, const std::regex & exp);
    bool quick_check(StringBounds & bounds, const std::string & word);
    bool parse_ident(StringBounds & bounds, std::string & result);
    bool parse_string(StringBounds & bounds, std::string & result);
    void skipWhitespaces(StringBounds & bounds);

    // Any Error, concerning a CustomScript
    class EScript : public Exception
    {
    public:
        EScript(std::string msg);
    };

    // Error while compilation, preventing the adventure from being started
    class ECompile : public EScript
    {
    public:
        ECompile(std::string msg);
    };

    class ENoMatch : public ECompile
    {
    public:
        ENoMatch();
    };

    // Error during runtime, e.g. evaluating a missing AdventureObject
    class ERuntime : public EScript
    {
    public:
        ERuntime(std::string msg);
    };

    // Error, because string can't get resolved into an AdventureObject 
    class EObjectEvaluation : public ERuntime
    {
    public:
        EObjectEvaluation(std::string identifier);
    };

    class EObjectTypeConflict : public ERuntime
    {
    public:
        EObjectTypeConflict(const AdventureObject & object, std::string expectedType);
    };

    class EItemTypeConflict : public EObjectTypeConflict
    {
    public:
        EItemTypeConflict(const AdventureObject & object);
    };

    class ERoomTypeConflict : public EObjectTypeConflict
    {
    public:
        ERoomTypeConflict(const AdventureObject & object);
    };

    class ELocationTypeConflict : public EObjectTypeConflict
    {
    public:
        ELocationTypeConflict(const AdventureObject & object);
    };

    class EPrepositionMissing : public ERuntime
    {
    public:
        EPrepositionMissing(const Location & location, std::string preposition);
    };

    class ELocationMissing : public ERuntime
    {
    public:                                                   
        ELocationMissing(const Location & location, const Room & room);
    };

    // Error when getting the User-Input parameter for a non existent parameter
    class EUnknownParam : public ERuntime
    {
    public:
        EUnknownParam(std::string param);
    };

    class ERunWithUnknownObjectType : public Exception
    {
    public:
        ERunWithUnknownObjectType(AdventureObject & object);
    };

}
