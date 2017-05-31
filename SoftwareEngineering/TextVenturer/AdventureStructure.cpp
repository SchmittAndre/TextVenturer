#include "stdafx.h"

#include "AliasList.h"

#include "AdventureStructure.h"

using namespace AdventureStructure;

AdventureStructure::BaseNode::BaseNode(std::string name)
    : name(name)
{
}

BaseNode::BaseNode(std::string name, ListNode & parent)
    : BaseNode(name)
{
    this->parent = &parent;
    parent.add(*this);
}

BaseNode::~BaseNode()
{
    if (parent)
        parent->del(*this);
}

std::string BaseNode::getName() const
{
    return name;
}

bool AdventureStructure::BaseNode::hasParent()
{
    return true;
}

ListNode & BaseNode::getParent()
{
    return *parent;
}

std::string BaseNode::getFullPath()
{
    if (parent)
        return parent->getFullPath() + "/" + name;
    else
        return name;
}

size_t AdventureStructure::BaseNode::getDepth()
{
    if (parent)
        return parent->getDepth() + 1;
    else
        return 0;
}

void AdventureStructure::BaseNode::remove()
{
    delete this;
}

std::string AdventureStructure::BaseNode::getTypeName() const
{
    return generateTypeName();
}

std::string AdventureStructure::BaseNode::generateTypeName()
{
    return "BaseNode";
}

void ListNode::del(BaseNode & node)
{
    ref_vector<BaseNode>::iterator pos = find(nodes.begin(), nodes.end(), node);
    if (pos == nodes.end())
        throw(ENodeDoesNotExist, *this, node);
    nodes.erase(pos);
}

void AdventureStructure::ListNode::delNoExcept(std::string name)
{
    auto pos = nodes.begin();
    for (; pos != nodes.end(); pos++)
        if (pos->get().getName() == name)
            break;
    if (pos != nodes.end())
        nodes.erase(pos);
}

bool AdventureStructure::ListNode::hasChild(std::string name) const
{
    for (auto & node : nodes)
        if (node.get().getName() == name)
            return true;
    return false;
}

AdventureStructure::ListNode::ListNode(std::string name) 
    : BaseNode(name)
{
}

ListNode::ListNode(std::string name, ListNode & parent)
    : BaseNode(name, parent)
{
}

ListNode::~ListNode()
{
    for (size_t i = nodes.size() - 1; i != std::string::npos ; i--)
        delete &nodes[i];
}

void ListNode::add(BaseNode & node)
{
    if (hasChild(node.getName()))
        throw(ENodeExistsAlready, *this, node);
    nodes.push_back(node);
}

BaseNode & ListNode::get(std::string name) const
{
    for (auto & node : nodes)
        if (node.get().getName() == name)
            return node;
    throw(ENodeNotFound, *this, name);
}

ListNode & AdventureStructure::ListNode::getListNode(std::string name) const
{
    BaseNode& node = get(name);
    try
    {
        return dynamic_cast<ListNode&>(node);
    }
    catch (std::bad_cast)
    {
        throw(EWrongType, *this, node, ListNode::generateTypeName());
    }
}

StringListNode & AdventureStructure::ListNode::getStringListNode(std::string name, bool identifierList) const
{
    BaseNode& node = get(name);
    try
    {
        StringListNode& result = dynamic_cast<StringListNode&>(node);
        if (result.isIdentifierList() != identifierList)
            throw std::bad_cast();
        return result;
    }
    catch (std::bad_cast)
    {
        try
        {
            throw(EEmptyList, dynamic_cast<EmptyListNode&>(node));
        }
        catch (std::bad_cast)
        {
            throw(EWrongType, *this, node, StringListNode::generateTypeName(identifierList));
        }
    }
}

StringNode & AdventureStructure::ListNode::getStringNode(std::string name, StringNode::Type type) const
{    
    BaseNode& node = get(name);
    try
    {
        StringNode& result = dynamic_cast<StringNode&>(node);
        if (result.getType() != type)
            throw std::bad_cast();
    }
    catch (std::bad_cast)
    {
        throw(EWrongType, *this, node, StringNode::generateTypeName(type));
    }
}

std::string AdventureStructure::ListNode::getString(std::string name, StringNode::Type type) const
{
    return getStringNode(name, type).getValue();
}

bool AdventureStructure::ListNode::getBoolean(std::string name, bool required, bool defaultValue) const
{
    try
    {
        StringNode& node = getStringNode(name, StringNode::stIdent);
        std::string value = node.getValue();
        if (value == "true")
            return true;
        if (value == "false")
            return false;
        throw(EInvalidBoolValue, node, value);
    }
    catch (ENodeNotFound)
    {
        if (required)
            throw;
        return defaultValue;
    }
}

stringlist AdventureStructure::ListNode::getStringList(std::string name, bool identList) const
{
    stringlist result;

    StringListNode& node = getStringListNode(name, false);
    for (std::string value : node)
        result.push_back(value);
    
    return result;
}

AliasList AdventureStructure::ListNode::getAliases() const
{
    AliasList result;
    try
    {
        StringListNode& node = getStringListNode("Aliases", false);
        for (std::string value : node)
            result.add(value, false);
    }
    catch (ENodeNotFound) { }

    try
    {
        StringListNode& node = getStringListNode("PluralAliases", false);
        for (std::string value : node)
            result.add(value, true);
    }
    catch (ENodeNotFound) { }
    if (result.empty())
        throw(ENodeNotFound, *this, "Aliases/PluralAliases");
    return result;
}

bool AdventureStructure::ListNode::empty() const
{
    return nodes.empty();
}

size_t AdventureStructure::ListNode::getCount() const
{
    return nodes.size();
}

ref_vector<BaseNode>::iterator ListNode::begin() 
{
    return nodes.begin();
}

ref_vector<BaseNode>::iterator ListNode::end() 
{
    return nodes.end();
}

std::string AdventureStructure::ListNode::getTypeName() const
{
    return generateTypeName();
}

std::string AdventureStructure::ListNode::generateTypeName()
{
    return "NodeList";
}

std::string AdventureStructure::ListNode::getContentName()
{
    return "Nodes";
}

StringListNode::StringListNode(std::string name, ListNode & parent, bool identifierList)
    : BaseNode(name, parent)
{
    this->identifierList = identifierList;
}

void StringListNode::add(std::string name)
{
    items.push_back(name);
}

bool StringListNode::isIdentifierList() const
{
    return identifierList;
}

std::string& StringListNode::get(size_t index)
{
    return items[index];
}

size_t StringListNode::getCount() const
{
    return items.size();
}


stringlist::iterator StringListNode::begin()
{
    return items.begin();
}


stringlist::iterator StringListNode::end()
{
    return items.end();
}

std::string AdventureStructure::StringListNode::getTypeName() const
{
    return generateTypeName(identifierList);
}

std::string AdventureStructure::StringListNode::generateTypeName(bool identList)
{
    return identList ? "IdentList" : "StringList";
}

std::string AdventureStructure::StringListNode::getContentName(bool identList)
{
    return identList ? "Identifiers" : "Strings";
}

StringNode::StringNode(std::string name, ListNode & parent, std::string value, Type type)
    : BaseNode(name, parent)
{
    this->value = value;
    this->type = type;
}

std::string StringNode::getValue() const
{
    return value;
}

void StringNode::setValue(std::string value)
{
    this->value = value;
}


StringNode::Type StringNode::getType() const
{
    return type;
}

StringNode::operator std::string() const
{
    return value;
}

std::string AdventureStructure::StringNode::getTypeName() const
{
    return generateTypeName(type);
}

std::string AdventureStructure::StringNode::generateTypeName(Type type)
{
    switch (type)
    {
    case StringNode::stString:
        return "String";
    case StringNode::stCode:
        return "Code";
    case StringNode::stIdent:
        return "Identifier";
    default:
        return "Unknown";
    }
}

RootNode::RootNode(std::string name)
    : ListNode(name)
{
}

void RootNode::loadFromString(std::string text)
{
    size_t linenumber = 1;
    size_t offset = 1;
    size_t pos = 0;

    ListNode& currentParent = *this;

    //const std::string ident = "[a-zA-Z0-9_]+";
    const std::string spaces = "[ \n\r\t]*";
    const std::string spaces1 = "[ \n\r\t]+";
    const std::string any = "[^]*?";

    const std::string end = "end";
    const std::string codeStart = "\\/CODE";
    const std::string codeEnd = "/\\END";

    const std::string lineCommentQuick = "//";
    const std::string multilineCommentQuick = "{";
    
    // const std::regex spaces1Exp = std::regex(spaces1, std::regex_constants::optimize);
    // const std::regex lineCommentExp = std::regex("//[^\\n]*\\n?", std::regex_constants::optimize);
    // const std::regex multilineCommentExp = std::regex("\\{[^}]*\\}", std::regex_constants::optimize);
    // const std::regex identExp = std::regex(ident, std::regex_constants::optimize);
    // const std::regex codeExp = std::regex("\\\\/CODE" + spaces1 + "(" + any + ")" + spaces + "/\\\\END", std::regex_constants::optimize);
    
    // std::smatch matches;
    /*
    auto error = [&](std::string msg, std::string additional = "", bool showPath = true)
    {
        if (additional != "")
            additional = "\n> " + additional;
        if (showPath)
            additional += "\n\nPath: " + currentParent.getFullPath();
        ErrorDialog("Script Parsing", msg + " at line " + std::to_string(linenumber) + " column " + std::to_string(offset) + additional);
    };
    */
    auto updateLine = [&](std::string text)
    {
        linenumber += count(text.cbegin(), text.cend(), '\n');
        size_t lastNewline = text.find_last_of('\n');
        if (lastNewline == std::string::npos)
            offset += text.size();
        else
            offset = text.size() - lastNewline;
    };
    /*
    auto regex_check = [&](const std::regex& regexString)
    {
        return std::regex_search(text.cbegin() + pos, text.cend(), matches, regexString, std::regex_constants::match_continuous);
    };
    */
    auto quick_check = [&](const std::string& word)
    {
        return !text.compare(pos, word.length(), word);
    };
    /*
    auto regex_quick_check = [&](const std::regex& regexString, const std::string& quickCheckWord)
    {
        return quick_check(quickCheckWord) && 
            std::regex_search(text.cbegin() + pos, text.cend(), matches, regexString, std::regex_constants::match_continuous);
    };
    */
    auto check_code = [&](std::string& code)
    {
        if (!quick_check(codeStart))
            return false;
        size_t i = pos + codeStart.size();
        size_t j;

        while (i < text.size())
        {
            j = 0;
            while (text[i + j] == codeEnd[j])
            {
                j++;
                if (j == codeEnd.size())
                {
                    return true;
                }
            }
            j++;
            code += text.substr(i, j);
            i += j;
        }
        return false;
    };

    auto check_ident = [&](std::string& outIdent)
    {
        size_t i = pos;
        outIdent = "";
        while (text[i] >= 'a' && text[i] <= 'z' ||
            text[i] >= 'A' && text[i] <= 'Z' ||
            text[i] >= '0' && text[i] <= '9' ||
            text[i] == '_')
        {
            outIdent += text[i++];
        }
        return i != pos;
    };

    auto check_ident_length = [&](size_t& length)
    {
        size_t i = pos;
        while (text[i] >= 'a' && text[i] <= 'z' ||
            text[i] >= 'A' && text[i] <= 'Z' ||
            text[i] >= '0' && text[i] <= '9' ||
            text[i] == '_')
        {
            i++;
        }
        length = i - pos;
        return length != 0;
    };

    auto parseString = [&](std::string &result)
    {
        pos++;
        offset++;
        while (pos < text.size() && text[pos] != '"')
        {
            if (text[pos] == '\n' || text[pos] == '\r')
                throw(EStringParseError, currentParent, "Unexpected end of line in string \"" + result + "\"");

            if (text[pos] == '\\')
            {
                if (pos + 1 < text.size())
                {
                    switch (text[pos + 1])
                    {
                    case '"':
                        result += '"';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    default:
                        throw(EStringParseError, currentParent, std::string("Only \\\" and \\\\ escaping is supported, got \\") + text[pos + 1] + " in string \"" + result + "\"");
                    }
                }
                else
                    throw(EStringParseError, currentParent, "Unexpected end of file after \\ in string \"" + result + "\"");
                pos++;
                offset++;
            }
            else
                result += text[pos];
            pos++;
            offset++;
        }
        if (pos == text.size())
            throw(EStringParseError, currentParent, "Unexpected end of file after \\ in string \"" + result + "\"");
            
        pos++;
        offset++;
    };

    auto skipWhitespacesAndComments = [&]()
    {
        bool more = true;
        bool skippedSome = false;

        while (more)
        {
            // skip whitespaces

            if (text[pos] == ' ' ||
                text[pos] == '\n' ||
                text[pos] == '\t' ||
                text[pos] == '\r')
            {
                do
                {
                    if (text[pos] == '\n')
                    {
                        linenumber++;
                        offset = 1;
                    }
                    pos++;
                } while (text[pos] == ' ' ||
                    text[pos] == '\n' ||
                    text[pos] == '\t' ||
                    text[pos] == '\r');

                skippedSome = true;
                continue;
            }
            
            // ignore // comment
            if (quick_check(lineCommentQuick))
            {
                pos = text.find('\n', pos + 2);
                linenumber++;
                offset = 1;
                skippedSome = true;
                continue;
            }

            // ignore { comment }
            if (quick_check(multilineCommentQuick))
            {
                pos = text.find('}', pos + 1);
                linenumber++;
                offset = 1;
                skippedSome = true;
                continue;
            }
            
            more = false;
        } 
        return skippedSome;
    };

    while (pos < text.size())
    {
        if (skipWhitespacesAndComments())
            continue;

        // end
        if (quick_check(end))
        {
            if (currentParent.hasParent())
                currentParent = currentParent.getParent();
            else
                throw(EAdventureStructureParse, currentParent, "No matching \"IDENTIFIER:\" for \"end\"");

            pos += end.size();
            offset += end.size();
            continue;
        }

        // IDENTIFIER
        std::string key;
        if (check_ident(key))
        {
            BaseNode * duplicate;
            try
            {
                duplicate = &currentParent.get(key);
            }
            catch (ENodeNotFound)
            {
                duplicate = NULL;
            }
            
            pos += key.size();
            offset += key.size();

            skipWhitespacesAndComments();

            // IDENTIFIER =
            if (text[pos] == '=')
            {
                if (duplicate)
                    throw(ENodeExistsAlready, currentParent, *duplicate);

                pos++;
                offset++;
            
                skipWhitespacesAndComments();

                std::string code;
                if (check_code(code))
                {
                    // IDENFITIER = \/CODE text /\END
                    pos += code.size() + codeStart.size() + codeEnd.size();
                    updateLine(code);
                    size_t first = code.find_first_not_of(" \n\r\t");
                    if (first != std::string::npos)
                        code = code.substr(first, code.find_last_not_of(" \n\r\t") - first + 1);
                    new StringNode(key, currentParent, code, StringNode::stCode);
                    
                    continue;
                }
                if (text[pos] == '"')
                {
                    // IDENTIFIER = "text"
                    std::string result;
                    parseString(result);

                    new StringNode(key, currentParent, result, StringNode::stString);
                    continue;
                }
                std::string value;
                if (check_ident(value))
                {
                    pos += value.size();
                    offset += value.size();
                    new StringNode(key, currentParent, value, StringNode::stIdent);
                    continue;
                }

                throw(EAdventureStructureParse, currentParent, "Expected string, identifier or code-block for assignment for key \"" + key + "\"");
            }

            // IDENTIFIER:
            if (text[pos] == ':')
            {      
                pos++;
                offset++;

                skipWhitespacesAndComments();

                if (text[pos] == '"')
                {
                    if (duplicate)
                        throw(ENodeExistsAlready, currentParent, *duplicate);
                    
                    // IDENTIFIER: "test" "hallo" END  
                    StringListNode& node = *new StringListNode(key, currentParent, false);
                    do
                    {
                        // check if we really have another string
                        if (text[pos] != '"')
                            throw(EAdventureStructureParse, node, "Expected next string or \"end\"");
                        std::string result;
                        parseString(result);
                        node.add(result);
                        // skip whitespaces
                        if (!skipWhitespacesAndComments())
                            throw(EAdventureStructureParse, node, "Expected space after end of string \"" + result + "\"");
                    } while (!quick_check(end));
                    pos += end.size();
                    offset += end.size();
                    continue;
                }
                if (quick_check(end))
                {
                    // empty, can't define type
                    pos += end.size();
                    offset += end.size();
                    new EmptyListNode(key, currentParent);
                    continue;
                }
                size_t identLength;
                if (check_ident_length(identLength))
                {
                    size_t savepos = pos; 
                    size_t saveline = linenumber;

                    pos += identLength;

                    skipWhitespacesAndComments();
                    if (text[pos] == ':' || text[pos] == '=')
                    {
                        // IDENTIFIER: ID1: END ID2: END END 
                        // or
                        // IDENTIFIER: ID1 = "test" END  
                        if (duplicate)
                        {
                            try
                            {
                                currentParent = dynamic_cast<ListNode&>(*duplicate);
                            }
                            catch (std::bad_cast)
                            {
                                throw(EAdventureStructureParse, currentParent, "Cannot merge Identifier \"" + key + "\", since they are not of the same type!");
                            }
                        }
                        else
                        {
                            currentParent = *new ListNode(key, currentParent);
                        }
                        pos = savepos;
                        continue;
                    }

                    pos = savepos;
                    linenumber = saveline;

                    // IDENTIFIER: test hallo END      
                    StringListNode & node = *new StringListNode(key, currentParent, true);
                    std::string ident;
                    while (check_ident(ident))
                    {
                        pos += ident.size();
                        offset += ident.size();
                        if (ident == "end")
                            break;
                        node.add(ident);

                        if (!skipWhitespacesAndComments())
                            throw(EAdventureStructureParse, node, "Expected space after end of identifier \"" + ident + "\"");
                    }
                    continue;
                }
                continue;
            }

            throw(EAdventureStructureParse, currentParent, "Identifier \"" + key + "\" not followed by \":\" or \"=\"");
        }

        size_t begin = text.find_last_of('\n', pos);
        size_t end = text.find_first_of('\n', pos);
        if (begin == std::string::npos)
            begin = 0;
        else
            begin++;
        throw(EAdventureStructureParse, currentParent, "Unknown error at \"" + text.substr(begin, end - begin) + "\"");   
    }

    if (currentParent.hasParent())
    {
        if (currentParent.getDepth() == 1)
            throw(EAdventureStructureParse, currentParent, "Missing \"end\" at end of file");
        else
            throw(EAdventureStructureParse, currentParent, "Missing " + std::to_string(currentParent.getDepth()) + " \"end\"s at end of file");
    }
}

void RootNode::loadFromFile(std::wstring filename)
{
    std::ifstream file(filename);
    if (!file.good())
        throw(Exception, "Could not open file");
    std::stringstream stream;
    stream << file.rdbuf();
    return loadFromString(stream.str());
}

bool AdventureStructure::RootNode::hasParent()
{
    return false;
}

ListNode & AdventureStructure::RootNode::getParent()
{
    throw(ERootHasNoParent, *this);
}

std::string AdventureStructure::RootNode::getTypeName() const
{
    return generateTypeName();
}

std::string AdventureStructure::RootNode::generateTypeName()
{
    return "RootNode";
}

EmptyListNode::EmptyListNode(std::string name, ListNode & parent)
    : BaseNode(name, parent)
{
}

std::string AdventureStructure::EmptyListNode::getTypeName() const
{
    return generateTypeName();
}

std::string AdventureStructure::EmptyListNode::generateTypeName()
{
    return "Empty List";
}

AdventureStructure::EAdventureStructure::EAdventureStructure(const BaseNode & node, std::string msg)
    : Exception(msg)
    , node(node)

{
}

const BaseNode & AdventureStructure::EAdventureStructure::getNode() const
{
    return node;
}

AdventureStructure::EAdventureStructureParse::EAdventureStructureParse(const BaseNode & node, std::string msg)
    : EAdventureStructure(node, msg)
{
}

AdventureStructure::ENodeNotFound::ENodeNotFound(const ListNode & node, std::string name)
    : EAdventureStructure(node, "The node \"" + name + "\" could not be found")
{
}

AdventureStructure::EListEmpty::EListEmpty(const ListNode & node, std::string name)
    : EAdventureStructure(node, "The list \"" + name + "\" is empty")
{
}

AdventureStructure::EWrongType::EWrongType(const ListNode & node, const BaseNode & wrong, std::string expected)
    : EAdventureStructure(node, "The node \"" + wrong.getName() + "\" should be of type \"" + expected + "\", but is \"" + wrong.getTypeName() + "\"")
{
}

AdventureStructure::ENodeExistsAlready::ENodeExistsAlready(const ListNode & base, const BaseNode & node)
    : EAdventureStructure(base, "Node \"" + node.getName() + "\" is already a subnode of \"" + base.getName() + "\"")
{
}

AdventureStructure::EInvalidBoolValue::EInvalidBoolValue(const StringNode & node, std::string value)
    : EAdventureStructure(node, "\"" + value + "\" is not a valid boolean value [true/false]")
{
}

AdventureStructure::ERootHasNoParent::ERootHasNoParent(const BaseNode & node)
    : EAdventureStructure(node, "Cannot get the parent of the root node")
{
}

AdventureStructure::EStringParseError::EStringParseError(const BaseNode & node, std::string msg)
    : EAdventureStructureParse(node, msg)
{
}

AdventureStructure::ENodeDoesNotExist::ENodeDoesNotExist(const ListNode & base, const BaseNode & node)
    : EAdventureStructure(base, "Node \"" + node.getName() + "\" is not a subnode of \"" + base.getName() + "\"")
{
}

AdventureStructure::EEmptyList::EEmptyList(const EmptyListNode & node)
    : EAdventureStructure(node, "List node \"" + node.getName() + "\" is empty")
{
}
