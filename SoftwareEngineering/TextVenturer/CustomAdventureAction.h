#pragma once

#include "AdventureAction.h"
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
        StringBounds(const std::string & text);
        StringBounds(const std::string::const_iterator begin, const std::string::const_iterator end);
        StringBounds(const std::ssub_match & submatch);
    };

    struct ExpressionParseData
    {
        StringBounds& bounds;
        Script* script;
        ExpressionParseData(StringBounds& bounds, Script* script);
    };                 

    struct StatementParseData
    {
        StringBounds& bounds;
        ControlStatement* parent;
        Script* script;           
        StatementParseData(StringBounds & bounds, Script* script, ControlStatement* parent = NULL);
    };

    // --- Expressions ---
    template <typename ResultType>
    class Expression abstract
    {
    private:
        Script* script;
    public:
        Expression(Script* script) : script(script) {}
        Script* getScript() const { return script;  };
        const Command::Result &getParams() const { return script->getParams(); }
        CustomAdventureAction* getAction() const { return script->getAction(); }
        virtual ResultType evaluate() = 0;
    };

    // Evaluation Types
    class ObjectExpression abstract : public Expression<AdventureObject*>
    {
    public:
        ObjectExpression(Script* script) : Expression<AdventureObject*>(script) {}
        static ObjectExpression* TryParse(ExpressionParseData &data);

        typedef bool(*TryParseFunc)(ExpressionParseData&, ObjectExpression*&);
        static const TryParseFunc TryParseList[];
    };       

    class BoolExpression abstract : public Expression<bool>
    {
    public:
        BoolExpression(Script* script) : Expression<bool>(script) {}
        static BoolExpression* TryParse(ExpressionParseData &data);

        typedef bool(*TryParseFunc)(ExpressionParseData&, BoolExpression*&);
        static const TryParseFunc TryParseList[];
    };

    class StringExpression abstract : public Expression<std::string>
    {
    public:
        StringExpression(Script* script) : Expression<std::string>(script) {}
        static StringExpression* TryParse(ExpressionParseData &data);

        typedef bool(*TryParseFunc)(ExpressionParseData&, StringExpression*&);
        static const TryParseFunc TryParseList[];
    };

    // Evaluation > Object
    class IdentExpression : public ObjectExpression
    {
    private:
        std::string ident;
    public:
        IdentExpression(Script* script) : ObjectExpression(script) {}
        void setIdent(const std::string & ident);
        AdventureObject* evaluate();

        static bool TryParse(ExpressionParseData &data, ObjectExpression*& expr);
    };

    class ParamExpression : public ObjectExpression
    {
    private:
        std::string param;
    public:
        ParamExpression(Script* script) : ObjectExpression(script) {}
        void setParam(const std::string & param);
        AdventureObject* evaluate();

        static bool TryParse(ExpressionParseData &data, ObjectExpression*& expr);
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

        static bool TryParse(ExpressionParseData &data, BoolExpression*& expr);
    };

    class ConstBoolExpression : public BoolExpression
    {
    private:
        bool value;
    public:
        ConstBoolExpression(Script* script) : BoolExpression(script) {}
        void setValue(bool value);
        bool evaluate();

        static bool TryParse(ExpressionParseData &data, BoolExpression*& expr);
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

        static bool TryParse(ExpressionParseData &data, StringExpression*& expr);
    };

    class ConstStringExpression : public StringExpression
    {
    private:
        std::string text;
    public:
        ConstStringExpression(Script* script) : StringExpression(script) {}
        void setText(const std::string & text);
        std::string evaluate();

        static bool TryParse(ExpressionParseData &data, StringExpression*& expr);
    };

    class StringConcatExpression : public StringExpression
    {
    private:
        std::vector<StringExpression*> stringExpList;
    public:
        StringConcatExpression(Script* script) : StringExpression(script) {}
        void add(StringExpression* stringExp);
        std::string evaluate();

        static bool TryParse(ExpressionParseData &data, StringExpression*& expr);
    };

    // --- Statements ---          
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

        typedef bool(*TryParseFunc)(StatementParseData&, Statement*&); 
        static const TryParseFunc TryParseList[];
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
        std::string title;
        StatementParseData parseData;
        std::string::const_iterator codeBegin;
        bool success;
              
    public:
        Script(CustomAdventureAction* action, std::string code, std::string title);
        bool run(const Command::Result & params);
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;

        bool succeeded();        
        void error(std::string message);
    };

    static bool check_regex(StringBounds& bounds, std::smatch & matches, const std::regex & exp);
    static void skipWhitespaces(StringBounds& bounds);
    
    const std::string ws = "[ \n\r\t]";
    const std::string ws0 = ws + "*";
    const std::string ws1 = ws + "+";
    const std::string any = "[^]+?";

}
    
class CustomAdventureAction : public AdventureAction
{
private:
    CustomScript::Script script;

public:
    CustomAdventureAction(Adventure* adventure, std::string code, std::string title);
    bool run(const Command::Result & params = Command::Result());
};