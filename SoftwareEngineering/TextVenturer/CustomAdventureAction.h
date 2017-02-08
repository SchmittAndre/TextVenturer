#pragma once

#include "AdventureAction.h"
#include "Command.h"

class Adventure;
class AdventureObject;

class CustomAdventureAction : public AdventureAction
{
private:
    CustomScript::Script script;

public:
    CustomAdventureAction(Adventure* adventure, std::string code);
    bool run(const Command::Result & params = Command::Result()) const;
};

namespace CustomScript
{            
    // --- Statements ---
    class ControlStatement;

    class Statement
    {
    private:
        CustomAdventureAction* action;
        Statement* next;
        ControlStatement* parent;
    public:
        Statement(CustomAdventureAction* action);
        virtual ~Statement();
        virtual bool execute(); 
        void setNext(Statement* next);
        void setParent(ControlStatement* parent);
        ControlStatement* getParent();
    };

    class ControlStatement : public Statement
    {
    public:
        ControlStatement(CustomAdventureAction* action) : Statement(action) {}
    };

    class IfStatement : public ControlStatement
    {
    private:
        BoolExpression* condition;
        Statement* thenPart; 
        Statement* elsePart;      
    public:
        IfStatement(CustomAdventureAction* action);
        ~IfStatement();
        bool execute();
        void setThenPart(Statement* thenPart);
        void setElsePart(Statement* elsePart);
    };

    class LoopStatement abstract : public ControlStatement
    {
    private:
        bool breakFlag;
    public:
        LoopStatement(CustomAdventureAction* action) : ControlStatement(action) {}
        bool breakOccured();
        void doBreak();
        void preExecute();
    };

    class WhileStatement : public LoopStatement
    {
    private:
        BoolExpression* condition;
        Statement* nextLooped;
    public:
        WhileStatement(CustomAdventureAction* action) : LoopStatement(action) {}
        bool execute();
    };

    class DoWhileStatement : public LoopStatement
    {
    private:
        BoolExpression* condition;
        Statement* nextLooped;
    public:
        DoWhileStatement(CustomAdventureAction* action) : LoopStatement(action) {}
        bool execute();
    };
    
    class BreakStatement : public Statement
    {
    public:
        BreakStatement(CustomAdventureAction* action) : Statement(action) {}
        bool execute();
    };

    class ContinueStatement : public Statement
    {
    public:
        ContinueStatement(CustomAdventureAction* action) : Statement(action) {}
        bool execute();
    };

    class SkipStatement : public Statement
    {
    public:
        SkipStatement(CustomAdventureAction* action) : Statement(action) {}
        bool execute();
    };

    class Script
    {
    private:
        CustomAdventureAction* action;
        Statement* root;
        Command::Result* params;
              
    public:
        Script(CustomAdventureAction* action);
        bool run(const Command::Result & params);
        bool loadFromString(std::string code);
    };

    // --- Expressions ---

    class Expression
    {
    private:
        CustomAdventureAction* action;
    public:
        Expression(CustomAdventureAction* action);
        CustomAdventureAction* getAction();
    };

    class BoolExpression abstract : public Expression
    {
    public:
        virtual bool evaluate() = 0;
    };       

    class ParamExpression : public Expression
    {
    private:
        std::string param;
    public:
        virtual AdventureObject* evaluate();
    };
}
    