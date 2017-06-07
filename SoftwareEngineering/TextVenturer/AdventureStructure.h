#pragma once

namespace AdventureStructure
{
    class ListNode;
    class RootNode;

    struct LineInfo
    {
        size_t line;
        size_t col;
        size_t pos;

        LineInfo();
    };

    // BaseNode
    class BaseNode abstract
    {
    private:
        std::string name;
        ListNode * parent;
        LineInfo keyPos;

    protected:
        bool used;

        BaseNode(std::string name);

    public:
        BaseNode(std::string name, ListNode & parent, LineInfo keyPos);
        virtual ~BaseNode();
        std::string getName() const;
        virtual bool hasParent();
        virtual ListNode & getParent();
        std::string getFullPath() const;
        size_t getDepth();

        const LineInfo & getKeyLineInfo() const;

        void remove();

        void markAsUsed();
        bool isUsed();

        virtual std::string getTypeName() const;
        static std::string generateTypeName();
    };

    // EmptyListNode
    class EmptyListNode : public BaseNode
    {
    public:
        EmptyListNode(std::string name, ListNode & parent, LineInfo keyPos);

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
        LineInfo textPos;

    public:
        StringNode(std::string name, ListNode & parent, std::string value, Type type, LineInfo keyPos, LineInfo textPos);
        std::string getValue() const;
        void setValue(std::string value);
        Type getType() const;

        const LineInfo & getTextLineInfo() const;

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
        StringListNode(std::string name, ListNode & parent, bool identifierList, LineInfo keyPos);
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
        ListNode(std::string name, ListNode & parent, LineInfo keyPos);
        ~ListNode();

        void add(BaseNode & node);
        void del(BaseNode & node);
        void delAll();
        bool hasChild(std::string name) const;

        // try functions only throw EWrongType, otherwise return NULL
        BaseNode * tryGet(std::string name) const;
        ListNode * tryGetListNode(std::string name) const;
        StringListNode * tryGetStringListNode(std::string name, bool identifierList) const;
        StringNode * tryGetStringNode(std::string name, StringNode::Type type) const;

        BaseNode & get(std::string name) const;
        ListNode & getListNode(std::string name) const;
        StringListNode & getStringListNode(std::string name, bool identifierList) const;
        StringNode & getStringNode(std::string name, StringNode::Type type) const;

        std::string getString(std::string name, StringNode::Type type = StringNode::stString) const;
        bool getBoolean(std::string name, bool required = false, bool defaultValue = false) const;
        stringlist getStringList(std::string name, bool identList = false, bool oneRequired = false) const;
        AliasList getAliases() const;
         
        bool empty() const;
        size_t getCount() const;

        ref_vector<BaseNode>::iterator begin();
        ref_vector<BaseNode>::iterator end();

        std::string getTypeName() const;
        static std::string generateTypeName();
        static std::string getContentName();       

        ref_vector<BaseNode> getUnusedNodes() const;
    };

    // RootNode
    class RootNode : public ListNode
    {
    public:
        RootNode();
        RootNode(std::wstring filename);

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
        EAdventureStructure(const BaseNode & node, const std::string & msg);
        const BaseNode & getNode() const;
    };

    class EAdventureStructureParse : public EAdventureStructure
    {
    public:
        EAdventureStructureParse(const BaseNode & node, const std::string & msg);
    };

    class EStringParseError : public EAdventureStructureParse
    {
    public:
        EStringParseError(const BaseNode & node, const std::string & msg);
    };

    class ENodeNotFound : public EAdventureStructure
    {
    public:
        ENodeNotFound(const ListNode & node, const std::string & name);
    };

    class EListEmpty : public EAdventureStructure
    {
    public:
        EListEmpty(const ListNode & node, const std::string & name);
    };

    class EWrongType : public EAdventureStructure
    {
    public:
        EWrongType(const BaseNode & node, const std::string & expected);
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
        EInvalidBoolValue(const StringNode & node, const std::string & value);
    };

    class ERootHasNoParent : public EAdventureStructure
    {
    public:
        ERootHasNoParent(const BaseNode & node);
    };

    class EEmptyOrMissing : public EAdventureStructure
    {
    public:
        EEmptyOrMissing(const ListNode & node, const std::string & name);
    };
}

