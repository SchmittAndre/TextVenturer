#pragma once

namespace AdventureStructure
{
    class ListNode;

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

    class StringListNode : public BaseNode
    {
    private:
        strings items;
        bool identifierList;
    public:
        StringListNode(string name, ListNode* parent, bool identifier = false);
        void add(string name);
        void del(size_t index);
        bool isIdentifierList() const;
        string& operator[] (size_t index);
        size_t getCount();
    };

    class StringNode : public BaseNode
    {
    private:
        string value;
        bool code;
    public:
        StringNode(string name, ListNode* parent, string value, bool code = false);
        string getValue() const;
        void setValue(string value);
        bool isCode() const;
    };

    class RootNode : public ListNode
    {       
    public:
        RootNode();
        bool loadFromString(string text);
        bool loadFromFile(string filename);
    };
}

