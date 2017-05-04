#pragma once

namespace AdventureStructure
{
    class EmptyListNode;
    class ListNode;
    class StringNode;
    class StringListNode;
    class RootNode;

    // BaseNode
    class BaseNode abstract
    {
    private:
        std::string name;
        ListNode* parent;
    public:
        BaseNode(std::string name, ListNode* parent);
        virtual ~BaseNode();
        std::string getName() const;
        ListNode* getParent();
        bool named(std::string name) const;
        std::string getFullPath();
        size_t getDepth();

        operator EmptyListNode*();
        operator ListNode*();
        operator StringNode*();
        operator StringListNode*();
        operator RootNode*();

        static std::string getTypeName();
    };

    // EmptyListNode
    class EmptyListNode : public BaseNode
    {
    public:
        EmptyListNode(std::string name, ListNode* parent);

        static std::string getTypeName();
    };

    // ListNode
    class ListNode : public BaseNode
    {
    private:
        std::vector<BaseNode*> nodes;
    public:                   
        ListNode(std::string name, ListNode* parent);
        ~ListNode();
        bool add(BaseNode* node);
        bool del(BaseNode* node);
        BaseNode* get(size_t index) const;
        BaseNode* get(std::string name) const;

        bool empty() const;
        size_t getCount() const;

        std::vector<BaseNode*>::iterator begin();
        std::vector<BaseNode*>::iterator end();

        static std::string getTypeName(); 
        static std::string getContentName();
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
        std::string value;
        Type type;
    public:
        StringNode(std::string name, ListNode* parent, std::string value, Type type);
        std::string getValue() const;
        void setValue(std::string value);
        Type getType() const;

        operator std::string() const;

        static std::string getTypeName(Type type);
    };

    // StringListNode
    class StringListNode : public BaseNode
    {
    private:
        stringlist items;
        bool identifierList;
    public:
        StringListNode(std::string name, ListNode* parent, bool identifierList);
        void add(std::string name);
        bool isIdentifierList() const;
        std::string& get(size_t index);
        size_t getCount() const;

        stringlist::iterator begin();
        stringlist::iterator end();

        static std::string getTypeName(bool identList);
        static std::string getContentName(bool identList);
    };        

    // RootNode
    class RootNode : public ListNode
    {       
    public:
        RootNode(std::string name = "root");
        bool loadFromString(std::string text);
        bool loadFromFile(std::wstring filename);  

        static std::string getTypeName();
    };
}

