#pragma once

#include "Command.h"

class Adventure;
class AdventureObject;
class CustomAdventureAction;

namespace CustomScript
{
    class ControlStatement;
    class Script;

    struct StringBounds
    {
        std::string::const_iterator begin;
        const std::string::const_iterator end;
        void advance(size_t amount, bool seekNext = true);
        StringBounds(const std::string & text);
        StringBounds(std::string::const_iterator begin, std::string::const_iterator end);
        StringBounds(const std::ssub_match & submatch);
    };                                      

    struct ParseData
    {
        StringBounds bounds;
        ControlStatement* parent;
        Script* script;
        bool skipLogicOp;
        ParseData(StringBounds bounds, Script* script, ControlStatement* parent = NULL);
    };

    class ParamExpression;

    // --- Expressions ---
    class Expression abstract
    {
    private:
        Script* script;
    public:
        Expression(Script* script) : script(script) {}
        virtual ~Expression() {}
        Script* getScript() const;
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;
    };

    enum ExpressionType {
        etObject,
        etBool,
        etString,
        etIdent,
        EXPRESSION_TYPE_COUNT
    };

    template <typename ResultType>
    class TypedExpression abstract : public Expression
    {
    public:
        TypedExpression(Script* script) : Expression(script) {}
        virtual ResultType evaluate() = 0;
        virtual ExpressionType getType() = 0;
    };

    // Evaluation Types
    class ObjectExpression abstract : public TypedExpression<AdventureObject*>
    {
    public:
        ObjectExpression(Script* script) : TypedExpression<AdventureObject*>(script) {}
        static ObjectExpression* TryParse(ParseData &data);
        ExpressionType getType();

        typedef bool(*TryParseFunc)(ParseData&, ObjectExpression*&);
        static const TryParseFunc TryParseList[];
    };

    class StringExpression abstract : public TypedExpression<std::string>
    {
    public:
        StringExpression(Script* script) : TypedExpression<std::string>(script) {}
        static StringExpression* TryParse(ParseData &data);
        ExpressionType getType();

        typedef bool(*TryParseFunc)(ParseData&, StringExpression*&);
        static const TryParseFunc TryParseList[];
    };

    class BoolExpression abstract : public TypedExpression<bool>
    {
    public:
        BoolExpression(Script* script) : TypedExpression<bool>(script) {}
        static BoolExpression* TryParse(ParseData &data);
        ExpressionType getType();

        typedef bool(*TryParseFunc)(ParseData&, BoolExpression*&);
        static const TryParseFunc TryParseList[];
    };

    // Evaluation > Object
    class IdentExpression : public ObjectExpression
    {
    private:
        std::string identifier;
    public:
        IdentExpression(Script* script) : ObjectExpression(script) {}
        AdventureObject* evaluate();

        static bool TryParse(ParseData &data, ObjectExpression*& expr);
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
        ObjectExpression* objectExp;
        bool startOfSentence;
        GenerateType type;

    public:
        ObjectToStringExpression(Script* script);
        ~ObjectToStringExpression();
        std::string evaluate();

        static bool TryParse(ParseData &data, StringExpression*& expr);
    };

    class ConstStringExpression : public StringExpression
    {
    private:
        std::string text;
    public:
        ConstStringExpression(Script* script) : StringExpression(script) {}
        std::string evaluate();

        static bool TryParse(ParseData &data, StringExpression*& expr);
    };

    class StringConcatExpression : public StringExpression
    {
    private:
        std::vector<StringExpression*> stringExpList;
    public:
        StringConcatExpression(Script* script) : StringExpression(script) {}
        ~StringConcatExpression();
        std::string evaluate();

        static StringConcatExpression* TryParse(ParseData &data);
    };

    class ParamExpression : public StringExpression
    {
    private:
        std::string param;
    public:
        ParamExpression(Script* script) : StringExpression(script) {}
        std::string evaluate();

        static bool TryParse(ParseData &data, StringExpression*& expr);
    };

    class IdentAsStringExpression : public StringExpression
    {
    private:
        std::string identString;
    public:
        IdentAsStringExpression(Script* script) : StringExpression(script) {}
        std::string evaluate();

        static IdentAsStringExpression* TryParse(ParseData &data);
    };

    // Evaluation > Bool  
    class ParamIsIdentExpression : public BoolExpression
    {
    private:
        ParamExpression* paramExp;
        IdentExpression* identExp;
    public:
        ParamIsIdentExpression(Script* script);
        ~ParamIsIdentExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
    };

    class ConstBoolExpression : public BoolExpression
    {
    private:
        bool value;
    public:
        ConstBoolExpression(Script* script) : BoolExpression(script) {}
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
    };

    class PlayerHasItemExpression : public BoolExpression
    {
    private:
        ObjectExpression* itemExp;
    public:
        PlayerHasItemExpression(Script* script);
        ~PlayerHasItemExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
    };

    class BracketExpression : public BoolExpression
    {
    private:
        BoolExpression* boolExp;
    public:
        BracketExpression(Script* script);
        ~BracketExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
    };

    class LogicNotExpression : public BoolExpression
    {
    private:
        BoolExpression* boolExp;
    public:
        LogicNotExpression(Script* script);
        ~LogicNotExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
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
        BoolExpression* boolExp1;
        BoolExpression* boolExp2;
        LogicalOperation operation;
    public:
        LogicOpExpression(Script* script);
        ~LogicOpExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
    };

    class LocationHasItemExpression : public BoolExpression
    {
    private:
        ObjectExpression* locationExp;
        ObjectExpression* itemExp;
        IdentAsStringExpression* prepositionExp;
    public:
        LocationHasItemExpression(Script* script);
        ~LocationHasItemExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
    };

    class ObjectFlagTestExpression : public BoolExpression
    {
    private:
        ObjectExpression* objectExp;
        IdentAsStringExpression* flagExp;

    public:
        ObjectFlagTestExpression(Script* script);
        ~ObjectFlagTestExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);    
    };

    class GlobalFlagTestExpression : public BoolExpression
    {
    private:
        IdentAsStringExpression* flagExp;

    public:
        GlobalFlagTestExpression(Script* script);
        ~GlobalFlagTestExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
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
    private:
        Statement* next;
        ControlStatement* parent;
        Script* script;
    public:
        Statement();
        virtual ~Statement();
        void setNext(Statement* next);
        void setParent(ControlStatement* parent);
        void setScript(Script* script);
        ControlStatement* getParent();
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;
        virtual bool execute();

        ParseResult parse(ParseData& data, ControlStatement* parent);

        typedef bool(*TryParseFunc)(ParseData&, Statement*&);
        static const TryParseFunc TryParseList[];
    };

    class ControlStatement abstract : public Statement
    {
    protected:
        BoolExpression* condition;
        bool evaluateCondition();
    public:
        ControlStatement();
        ~ControlStatement();
    };

    class IfStatement : public ControlStatement
    {
    private:
        Statement* thenPart;
        Statement* elsePart;
    public:
        IfStatement();
        ~IfStatement();
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
    };

    class SwitchStatement : public ControlStatement
    {
    public:
        struct CaseSection
        {
            IdentExpression* ident;
            Statement* statement;
            CaseSection(IdentExpression* ident, Statement* statement);
        };

    private:
        ParamExpression* switchPart;
        std::vector<CaseSection> caseParts;
        Statement* elsePart;
    public:
        SwitchStatement();
        ~SwitchStatement();
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
    };

    class LoopStatement abstract : public ControlStatement
    {
    private:
        bool breakFlag;
        Statement* loopPart;
    protected:
        bool executeLoopPart();
        bool breakOccured();
        void preExecute();
    public:
        LoopStatement();
        ~LoopStatement();
        void doBreak();
    };

    class WhileStatement : public LoopStatement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
    };

    class DoWhileStatement : public LoopStatement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
    };

    class BreakStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
    };

    class ContinueStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
    };

    class SkipStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
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
            ptGlobalSet,
            ptGlobalClear,

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
        ~ProcedureStatement();
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
    };

    class Script
    {
    private:
        CustomAdventureAction* action;
        Statement root;
        const Command::Result* params;
        std::string title;
        ParseData parseData;
        std::string::const_iterator codeBegin;
        bool success;

        tags requiredParams;

    public:
        Script(CustomAdventureAction* action, std::string code, std::string title);
        bool run(const Command::Result & params);
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;

        tags& getRequiredParams();

        bool succeeded() const;
        void error(std::string message) const;
    };

    bool check_regex(StringBounds bounds, std::smatch & matches, const std::regex & exp);
    void skipWhitespaces(StringBounds& bounds);

    const std::string ws = "[ \n\r\t]";
    const std::string ws0 = ws + "*";
    const std::string ws1 = ws + "+";
    const std::string any = "[^]+?";
    const std::string ident = "[a-zA-Z0-9_]+";

}
