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
        const std::string& text;
        size_t pos;
        size_t end;
        void advance(size_t amount, bool seekNext = true);
        StringBounds(const std::string & text, size_t pos = 0, size_t end = std::string::npos);
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
        Script* script;
    public:
        Expression(Script* script) : script(script) {}
        virtual ~Expression() {}
        Script* getScript() const;
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;

        static Expression* loadTyped(FileStream & stream, ExpressionType type, Script * script);
        virtual void save(FileStream & stream) = 0;
        virtual void load(FileStream & stream, Script * script) = 0;
    };

    template <typename ResultType>
    class TypedExpression abstract : public Expression
    {
    public:
        TypedExpression(Script* script) : Expression(script) {}
        virtual ResultType evaluate() = 0;
        virtual ExpressionType getResultType() = 0;
    };

    // Evaluation Types
    class ObjectExpression abstract : public TypedExpression<AdventureObject*>
    {
    protected:
        enum Type
        {
            etIdent
        };

        virtual Type getType() = 0;

    public:
        ObjectExpression(Script* script) : TypedExpression<AdventureObject*>(script) {}
        static ObjectExpression* TryParse(ParseData &data);
        ExpressionType getResultType();

        typedef bool(*TryParseFunc)(ParseData&, ObjectExpression*&);
        static const TryParseFunc TryParseList[];

        static ObjectExpression* loadTyped(FileStream & stream, Script * script);
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
        StringExpression(Script* script) : TypedExpression<std::string>(script) {}
        static StringExpression* TryParse(ParseData &data);
        ExpressionType getResultType();

        typedef bool(*TryParseFunc)(ParseData&, StringExpression*&);
        static const TryParseFunc TryParseList[];

        static StringExpression* loadTyped(FileStream & stream, Script * script);
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
        BoolExpression(Script* script) : TypedExpression<bool>(script) {}
        static BoolExpression* TryParse(ParseData &data);
        ExpressionType getResultType();

        typedef bool(*TryParseFunc)(ParseData&, BoolExpression*&);
        static const TryParseFunc TryParseList[];

        static BoolExpression* loadTyped(FileStream & stream, Script * script);
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
        IdentExpression(Script* script) : ObjectExpression(script) {}
        AdventureObject* evaluate();

        static bool TryParse(ParseData &data, ObjectExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
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

    protected:
        Type getType();

    public:
        ObjectToStringExpression(Script* script);
        ~ObjectToStringExpression();
        std::string evaluate();

        static bool TryParse(ParseData &data, StringExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class ConstStringExpression : public StringExpression
    {
    private:
        std::string text;

    protected:
        Type getType();

    public:
        ConstStringExpression(Script* script) : StringExpression(script) {}
        std::string evaluate();

        static bool TryParse(ParseData &data, StringExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class StringConcatExpression : public StringExpression
    {
    private:
        std::vector<StringExpression*> stringExpList;

    protected:
        Type getType();

    public:
        StringConcatExpression(Script* script) : StringExpression(script) {}
        ~StringConcatExpression();
        std::string evaluate();

        static StringConcatExpression* TryParse(ParseData &data);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class ParamExpression : public StringExpression
    {
    private:
        std::string param;

    protected:
        Type getType();

    public:
        ParamExpression(Script* script) : StringExpression(script) {}
        std::string evaluate();

        static bool TryParse(ParseData &data, StringExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class IdentAsStringExpression : public StringExpression
    {
    private:
        std::string identString;

    protected:
        Type getType();

    public:
        IdentAsStringExpression(Script* script) : StringExpression(script) {}
        std::string evaluate();

        static IdentAsStringExpression* TryParse(ParseData &data);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    // Evaluation > Bool  
    class ParamIsIdentExpression : public BoolExpression
    {
    private:
        ParamExpression* paramExp;
        IdentExpression* identExp;

    protected:
        Type getType();

    public:
        ParamIsIdentExpression(Script* script);
        ~ParamIsIdentExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);
        
        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class ConstBoolExpression : public BoolExpression
    {
    private:
        bool value;

    protected:
        Type getType();

    public:
        ConstBoolExpression(Script* script) : BoolExpression(script) {}
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class PlayerHasItemExpression : public BoolExpression
    {
    private:
        ObjectExpression* itemExp;

    protected:
        Type getType();

    public:
        PlayerHasItemExpression(Script* script);
        ~PlayerHasItemExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class BracketExpression : public BoolExpression
    {
    private:
        BoolExpression* boolExp;

    protected:
        Type getType();

    public:
        BracketExpression(Script* script);
        ~BracketExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class LogicNotExpression : public BoolExpression
    {
    private:
        BoolExpression* boolExp;

    protected:
        Type getType();

    public:
        LogicNotExpression(Script* script);
        ~LogicNotExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
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

    protected:
        Type getType();

    public:
        LogicOpExpression(Script* script);
        ~LogicOpExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class LocationHasItemExpression : public BoolExpression
    {
    private:
        ObjectExpression* locationExp;
        ObjectExpression* itemExp;
        IdentAsStringExpression* prepositionExp;

    protected:
        Type getType();

    public:
        LocationHasItemExpression(Script* script);
        ~LocationHasItemExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class ObjectFlagTestExpression : public BoolExpression
    {
    private:
        ObjectExpression* objectExp;
        IdentAsStringExpression* flagExp;

    protected:
        Type getType();

    public:
        ObjectFlagTestExpression(Script* script);
        ~ObjectFlagTestExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);    

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class GlobalFlagTestExpression : public BoolExpression
    {
    private:
        IdentAsStringExpression* flagExp;

    protected:
        Type getType();

    public:
        GlobalFlagTestExpression(Script* script);
        ~GlobalFlagTestExpression();
        bool evaluate();

        static bool TryParse(ParseData &data, BoolExpression*& expr);

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
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

        static Statement* loadTyped(FileStream & stream, Script * script);

        virtual Type getType();
        virtual void save(FileStream & stream);
        virtual void load(FileStream & stream, Script * script);
    };

    class ControlStatement abstract : public Statement
    {
    };

    class ConditionalStatement abstract : public ControlStatement
    {
    protected:
        BoolExpression* condition;
        bool evaluateCondition();

    public:
        ConditionalStatement();
        ~ConditionalStatement();

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class IfStatement : public ConditionalStatement
    {
    private:
        Statement* thenPart;
        Statement* elsePart;
    
    public:
        IfStatement();
        ~IfStatement();
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);

        Type getType();
        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
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

        Type getType();
        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class LoopStatement abstract : public ConditionalStatement
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

        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class WhileStatement : public LoopStatement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
        
        Type getType();
    };

    class DoWhileStatement : public LoopStatement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
        
        Type getType();    
    };

    class BreakStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
        
        Type getType();
    };

    class ContinueStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);     
        
        Type getType();
    };

    class SkipStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);
        
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
        ~ProcedureStatement();
        bool execute();
        static bool TryParse(ParseData& data, Statement*& stmt);

        Type getType();
        void save(FileStream & stream);
        void load(FileStream & stream, Script * script);
    };

    class Script
    {
    private:
        CustomAdventureAction* action;
        Statement* root;
        const Command::Result* params;
        std::string title;
        std::string* code; 
        ParseData* parseData;
        bool success;

        tags requiredParams;

    public:
        Script(CustomAdventureAction* action, FileStream & stream);
        Script(CustomAdventureAction* action, std::string code, std::string title);
        ~Script();
        bool run(const Command::Result & params);
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;

        tags& getRequiredParams();

        const std::string &getCode();

        bool succeeded() const;
        void error(std::string message) const;

        void save(FileStream & stream);
    };

    // bool check_regex(StringBounds bounds, std::smatch & matches, const std::regex & exp);
    bool quick_check(StringBounds& bounds, const std::string& word);
    bool parse_ident(StringBounds& bounds, std::string& result);
    bool parse_string(StringBounds& bounds, std::string& result);
    void skipWhitespaces(StringBounds& bounds);

    // const std::string ws = "[ \n\r\t]";
    // const std::string ws0 = ws + "*";
    // const std::string ws1 = ws + "+";
    // const std::string any = "[^]+?";
    // const std::string ident = "[a-zA-Z0-9_]+";

}
