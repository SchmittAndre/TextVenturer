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
        ParseData(StringBounds bounds, Script* script, ControlStatement* parent = NULL);
    };

    class ParamExpression;

    // --- Expressions ---
    template <typename ResultType>
    class Expression abstract
    {
    private:
        Script* script;
    public:
        Expression(Script* script) : script(script) {}
        virtual ~Expression() {};
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
        static ObjectExpression* TryParse(ParseData &data);

        typedef bool(*TryParseFunc)(ParseData&, ObjectExpression*&);
        static const TryParseFunc TryParseList[];
    };       

    class BoolExpression abstract : public Expression<bool>
    {
    public:
        BoolExpression(Script* script) : Expression<bool>(script) {}
        static BoolExpression* TryParse(ParseData &data);

        typedef bool(*TryParseFunc)(ParseData&, BoolExpression*&);
        static const TryParseFunc TryParseList[];
    };

    class StringExpression abstract : public Expression<std::string>
    {
    public:
        StringExpression(Script* script) : Expression<std::string>(script) {}
        
        typedef bool(*TryParseFunc)(ParseData&, StringExpression*&);
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

    class WriteStatement : public Statement
    {
    private:
        StringExpression* stringExp;
    public:
        WriteStatement();
        ~WriteStatement();
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
              
    public:
        Script(CustomAdventureAction* action, std::string code, std::string title);
        bool run(const Command::Result & params);
        const Command::Result &getParams() const;
        CustomAdventureAction* getAction() const;

        bool succeeded() const;        
        void error(std::string message) const;
    };

    bool check_regex(StringBounds bounds, std::smatch & matches, const std::regex & exp);
    void skipWhitespaces(StringBounds& bounds);
    
    const std::string ws = "[ \n\r\t]";
    const std::string ws0 = ws + "*";
    const std::string ws1 = ws + "+";
    const std::string any = "[^]+?";
    const std::string ident = "[a-zA-Z0-9]+";

}
    
class CustomAdventureAction : public AdventureAction
{
private:
    CustomScript::Script script;

public:
    CustomAdventureAction(Adventure* adventure, std::string code, std::string title);
    bool compileSucceeded() const;
    bool run(const Command::Result & params = Command::Result());
};