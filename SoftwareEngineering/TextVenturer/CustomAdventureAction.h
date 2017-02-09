#pragma once

#include "AdventureAction.h"
#include "Command.h"

class Adventure;
class AdventureObject;

namespace CustomScript
{            
    struct StringBounds
    {
        std::string::const_iterator begin;
        const std::string::const_iterator end;
        StringBounds(const std::string & text);
        StringBounds(const std::string::const_iterator begin, const std::string::const_iterator end);
        StringBounds(const std::ssub_match & submatch);
    };

    struct StatementParseData
    {
        StringBounds bounds;
        ControlStatement* parent;
        Script* script;           
        StatementParseData(const StringBounds & bounds, Script* script, ControlStatement* parent = NULL);
    };

    class Script;

    // --- Expressions ---
    // Base
    template <typename ResultType>
    class Expression abstract
    {
    private:
        Script* script;
    public:
        Expression(Script* script) : script(script) {}
        const Command::Result &getParams() const { return script->getParams(); }
        CustomAdventureAction* getAction() const { return script->getAction(); }
        virtual ResultType evaluate() = 0;
    };

    // Evaluation Types
    typedef Expression<AdventureObject*> ObjectExpression;
    typedef Expression<bool> BoolExpression;
    typedef Expression<std::string> StringExpression;

    // Evaluation > Object
    class IdentExpression : public ObjectExpression
    {
    private:
        std::string ident;
    public:
        IdentExpression(Script* script) : ObjectExpression(script) {}
        void setIdent(const std::string & ident);
        AdventureObject* evaluate();
    };

    class ParamExpression : public ObjectExpression
    {
    private:
        std::string param;
    public:
        ParamExpression(Script* script) : ObjectExpression(script) {}
        void setParam(const std::string & param);
        AdventureObject* evaluate();
    };

    // Evaluation > Bool  
    class ParamIsIdentExpression : public BoolExpression
    {
    private:
        ParamExpression* paramExp;
        IdentExpression* identExp;
    public:
        ParamIsIdentExpression(Script* script) : BoolExpression(script) {}
        void setParamExpression(ParamExpression* paramExp);
        void setIdentExpression(IdentExpression* identExp);
        bool evaluate();
    };

    class ConstBoolExpression : public BoolExpression
    {
    private:
        bool value;
    public:
        ConstBoolExpression(Script* script) : BoolExpression(script) {}
        void setValue(bool value);
        bool evaluate();
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
        ObjectToStringExpression(Script* script) : StringExpression(script) {}
        void setObjectExp(ObjectExpression* objectExp);
        void setStartOfSentence(bool startOfSentence);
        void setGenerateType(GenerateType type);
        std::string evaluate();
    };

    class ConstStringExpression : public StringExpression
    {
    private:
        std::string text;
    public:
        ConstStringExpression(Script* script) : StringExpression(script) {}
        void setText(const std::string & text);
        std::string evaluate();
    };

    class StringConcatExpression : public StringExpression
    {
    private:
        std::vector<StringExpression*> stringExpList;
    public:
        StringConcatExpression(Script* script) : StringExpression(script) {}
        void add(StringExpression* stringExp);
        std::string evaluate();
    };

    // --- Statements ---
    class ControlStatement;    

    class Statement
    {
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

        bool parse(StatementParseData& data);
        static bool check_regex(StringBounds& bounds, std::smatch & matches, const std::regex & exp);
        static void skipWhitespaces(StringBounds& bounds);
        static const std::string ws;
        static const std::string ws0;
        static const std::string ws1;
        static const std::string any;
        static const std::string capture_any;
    };

    class ControlStatement abstract : public Statement
    {
    private:
        BoolExpression* condition;
    protected:
        bool evaluateCondition();
    public:
        ControlStatement();
        ~ControlStatement();
        void setCondition(BoolExpression* condition);
    };

    class IfStatement : public ControlStatement
    {
    private:
        Statement* thenPart; 
        Statement* elsePart;
    public:
        IfStatement();
        ~IfStatement();
        void setThenPart(Statement* thenPart);
        void setElsePart(Statement* elsePart);
        bool execute();
        static bool TryParse(StatementParseData& data, Statement*& stmt);
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
        void setLoopPart(Statement* loopPart);
        void doBreak();
    };

    class WhileStatement : public LoopStatement
    {
    public:
        bool execute();
        static bool TryParse(StatementParseData& data, Statement*& stmt);
    };

    class DoWhileStatement : public LoopStatement
    {
    public:
        bool execute();
        static bool TryParse(StatementParseData& data, Statement*& stmt);
    };
    
    class BreakStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(StatementParseData& data, Statement*& stmt);
    };

    class ContinueStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(StatementParseData& data, Statement*& stmt);
    };

    class SkipStatement : public Statement
    {
    public:
        bool execute();
        static bool TryParse(StatementParseData& data, Statement*& stmt);
    };

    class WriteStatement : public Statement
    {
    private:
        StringExpression* stringExp;
    public:
        WriteStatement();
        ~WriteStatement();
        void setStringExpression(StringExpression* stringExp);
        bool execute();
        static bool TryParse(StatementParseData& data, Statement*& stmt);
    };

    class Script
    {
    private:
        CustomAdventureAction* action;
        Statement root;
        const Command::Result* params;
              
    public:
        Script(CustomAdventureAction* action);
        bool run(const Command::Result & params);
        bool loadFromString(std::string code);
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;
    };

    typedef bool (*StatementTryParseFunc)(StatementParseData&, Statement*&);

    const StatementTryParseFunc StatementTryParseList[] = {
        IfStatement::TryParse,
        WhileStatement::TryParse,
        DoWhileStatement::TryParse,
        BreakStatement::TryParse,
        ContinueStatement::TryParse,
        SkipStatement::TryParse,
        WriteStatement::TryParse
    };
}
    
class CustomAdventureAction : public AdventureAction
{
private:
    CustomScript::Script script;

public:
    CustomAdventureAction(Adventure* adventure, std::string code);
    bool run(const Command::Result & params = Command::Result());
};