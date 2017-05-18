#pragma once

namespace AdventureStructure
{
    class ListNode;
    class RootNode;

    // BaseNode
    class BaseNode abstract
    {
    private:
        std::string name;
        ref_optional<ListNode> parent;

    protected:
        BaseNode(std::string name);

    public:
        BaseNode(std::string name, ListNode & parent);
        virtual ~BaseNode();
        std::string getName() const;
        bool hasParent();
        ListNode & getParent();
        std::string getFullPath();
        size_t getDepth();

        void remove();

        static std::string getTypeName();
    };

    // EmptyListNode
    class EmptyListNode : public BaseNode
    {
    public:
        EmptyListNode(std::string name, ListNode & parent);

        static std::string getTypeName();
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
        StringNode(std::string name, ListNode & parent, std::string value, Type type);
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
        StringListNode(std::string name, ListNode & parent, bool identifierList);
        void add(std::string name);
        bool isIdentifierList() const;
        std::string & get(size_t index);
        size_t getCount() const;

        stringlist::iterator begin();
        stringlist::iterator end();

        static std::string getTypeName(bool identList);
        static std::string getContentName(bool identList);
    };        

    // ListNode
    class ListNode : public BaseNode
    {
    private:
        std::vector<BaseNode*> nodes;

    protected:
        ListNode(std::string name);

    public:
        ListNode(std::string name, ListNode & parent);
        ~ListNode();

        void add(BaseNode & node);
        void del(BaseNode & node);
        bool hasChild(std::string name) const;

        BaseNode & get(std::string name) const;
        ListNode & getListNode(std::string name) const;
        StringListNode & getStringListNode(std::string name, bool identifierList) const;
        std::string & getString(std::string name, StringNode::Type type) const;
        bool getBoolean(std::string name, bool required = false, bool defaultValue = false) const;
        AliasList getAliasList(std::string name) const;
               
        bool empty() const;
        size_t getCount() const;

        std::vector<BaseNode*>::iterator begin();
        std::vector<BaseNode*>::iterator end();

        static std::string getTypeName();
        static std::string getContentName();
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

    class EAdventureStructure : public Exception
    {
    public:
        EAdventureStructure(std::string message);
    };

    class ENodeNotFound : public EAdventureStructure
    {
    public:
        ENodeNotFound(const ListNode & node, std::string name);
    };

    class EListEmpty : public EAdventureStructure
    {
    public:
        EListEmpty(const ListNode & node, std::string name);
    };

    class EWrongType : public EAdventureStructure
    {
    public:
        EWrongType(const ListNode & node, std::string name, std::string expected, std::string got);
    };

    class ENodeExistsAlready : public EAdventureStructure
    {
    public:
        ENodeExistsAlready(const ListNode & node, std::string name);
    };
}

