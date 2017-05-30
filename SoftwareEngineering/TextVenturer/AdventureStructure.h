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
        ListNode * parent;

    protected:
        BaseNode(std::string name);

    public:
        BaseNode(std::string name, ListNode & parent);
        virtual ~BaseNode();
        std::string getName() const;
        virtual bool hasParent();
        virtual ListNode & getParent();
        std::string getFullPath();
        size_t getDepth();

        void remove();

        virtual std::string getTypeName() const;
        static std::string generateTypeName();
    };

    // EmptyListNode
    class EmptyListNode : public BaseNode
    {
    public:
        EmptyListNode(std::string name, ListNode & parent);

        std::string getTypeName() const;
        static std::string generateTypeName();
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

        std::string getTypeName() const;
        static std::string generateTypeName(Type type);
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

        std::string getTypeName() const;
        static std::string generateTypeName(bool identList);
        static std::string getContentName(bool identList);
    };        

    // ListNode
    class ListNode : public BaseNode
    {
    private:
        ref_vector<BaseNode> nodes;

    protected:
        ListNode(std::string name);

    public:
        ListNode(std::string name, ListNode & parent);
        ~ListNode();

        void add(BaseNode & node);
        void del(BaseNode & node);
        void delNoExcept(std::string name);
        bool hasChild(std::string name) const;

        BaseNode & get(std::string name) const;
        ListNode & getListNode(std::string name) const;
        StringListNode & getStringListNode(std::string name, bool identifierList) const;
        StringNode & getStringNode(std::string name, StringNode::Type type) const;

        std::string getString(std::string name, StringNode::Type type = StringNode::stString) const;
        bool getBoolean(std::string name, bool required = false, bool defaultValue = false) const;
        stringlist getStringList(std::string name, bool identList = false) const;
        AliasList getAliases() const;
               
        bool empty() const;
        size_t getCount() const;

        ref_vector<BaseNode>::iterator begin();
        ref_vector<BaseNode>::iterator end();

        std::string getTypeName() const;
        static std::string generateTypeName();
        static std::string getContentName();
    };

    // RootNode
    class RootNode : public ListNode
    {
    public:
        RootNode(std::string name = "root");
        void loadFromString(std::string text);
        void loadFromFile(std::wstring filename);

        bool hasParent();
        ListNode & getParent();

        std::string getTypeName() const;
        static std::string generateTypeName();
    };

    class EAdventureStructure : public Exception
    {
    private:
        const BaseNode & node;
    public:
        EAdventureStructure(const BaseNode & node, std::string msg);
        const BaseNode & getNode() const;
    };

    class EAdventureStructureParse : public EAdventureStructure
    {
    public:
        EAdventureStructureParse(const BaseNode & node, std::string msg);
    };

    class EStringParseError : public EAdventureStructureParse
    {
    public:
        EStringParseError(const BaseNode & node, std::string msg);
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
        EWrongType(const ListNode & node, const BaseNode & wrong, std::string expected);
    };

    class ENodeExistsAlready : public EAdventureStructure
    {
    public:
        ENodeExistsAlready(const ListNode & base, const BaseNode & node);
    };

    class ENodeDoesNotExist : public EAdventureStructure
    {
    public:
        ENodeDoesNotExist(const ListNode & base, const BaseNode & node);
    };

    class EInvalidBoolValue : public EAdventureStructure
    {
    public:
        EInvalidBoolValue(const StringNode & node, std::string value);
    };

    class ERootHasNoParent : public EAdventureStructure
    {
    public:
        ERootHasNoParent(const BaseNode& node);
    };
}

