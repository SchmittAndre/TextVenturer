#pragma once

namespace AdventureStructure
{
    class ListNode;

    // BaseNode
    class BaseNode abstract
    {
    private:
        string name;
        ListNode* parent;
    public:
        BaseNode(string name, ListNode* parent);
        virtual ~BaseNode();
        string getName() const;
        ListNode* getParent();
    };

    // ListNode
    class ListNode : public BaseNode
    {
    private:
        vector<BaseNode*> nodes;
    public:                   
        ListNode(string name, ListNode* parent);
        ~ListNode();
        bool add(BaseNode* node);
        bool del(BaseNode* node);
        BaseNode* operator[] (size_t index) const;
        BaseNode* operator[] (string name) const; 
    };

    // StringListNode
    class StringListNode : public BaseNode
    {
    private:
        strings items;
        bool identifierList;
    public:
        StringListNode(string name, ListNode* parent, bool identifierList);
        void add(string name);
        void del(size_t index);
        bool isIdentifierList() const;
        string& operator[] (size_t index);
        size_t getCount();
    };

    // StringNode
    class StringNode : public BaseNode
    {
    public:
        enum Type
        {
            stString,
            stCode,
            stIdent
        };
    private:
        string value;
        Type type;
    public:
        StringNode(string name, ListNode* parent, string value, Type type);
        string getValue() const;
        void setValue(string value);
        Type getType() const;
    };

    // RootNode
    class RootNode : public ListNode
    {       
    public:
        RootNode();
        bool loadFromString(string text);
        bool loadFromFile(string filename);
    };
}

