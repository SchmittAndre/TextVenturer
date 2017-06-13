#include "stdafx.h"

#include "AliasList.h"

#include "AdventureStructure.h"

using namespace AdventureStructure;

AdventureStructure::BaseNode::BaseNode(std::string name)
    : name(name)
    , parent(NULL)
{
}

BaseNode::BaseNode(std::string name, ListNode & parent, LineInfo keyPos)
    : name(name)
    , parent(&parent)
    , keyPos(keyPos)
{
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

std::string BaseNode::getFullPath() const
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

const LineInfo & AdventureStructure::BaseNode::getKeyLineInfo() const
{
    return keyPos;
}

void AdventureStructure::BaseNode::remove()
{
    delete this;
}

void AdventureStructure::BaseNode::markAsUsed()
{
    used = true;
}

bool AdventureStructure::BaseNode::isUsed()
{
    return used;
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
    auto pos = std::find_if(nodes.begin(), nodes.end(), [&](BaseNode & a) 
    {
        return &node == &a;
    });
    if (pos == nodes.end())
        throw(ENodeDoesNotExist, *this, node);
    nodes.erase(pos);
}

void AdventureStructure::ListNode::delAll()
{
    for (size_t i = nodes.size() - 1; i != std::string::npos; i--)
        delete &nodes[i].get();
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

ListNode::ListNode(std::string name, ListNode & parent, LineInfo keyPos)
    : BaseNode(name, parent, keyPos)
{
}

ListNode::~ListNode()
{
    delAll();
}

void ListNode::add(BaseNode & node)
{
    if (hasChild(node.getName()))
        throw(ENodeExistsAlready, *this, node);
    nodes.push_back(node);
}

BaseNode * AdventureStructure::ListNode::tryGet(std::string name) const
{
    for (BaseNode & node : nodes)
        if (node.getName() == name)
        {
            node.markAsUsed();
            return &node;
        }
    return NULL;
}

ListNode * AdventureStructure::ListNode::tryGetListNode(std::string name) const
{   
    if (BaseNode * node = tryGet(name))
    {
        if (ListNode * typed = dynamic_cast<ListNode*>(node))
            return typed;
        if (dynamic_cast<EmptyListNode*>(node))
            return NULL;
        throw(EWrongType, *node, ListNode::generateTypeName());    
    }
    else
        return NULL;
}

StringListNode * AdventureStructure::ListNode::tryGetStringListNode(std::string name, bool identifierList) const
{
    if (BaseNode * node = tryGet(name))
    {
        if (StringListNode * typed = dynamic_cast<StringListNode*>(node))
        {
            if (typed->isIdentifierList() == identifierList)
                return typed;
            throw(EWrongType, *node, StringListNode::generateTypeName(identifierList));
        }
        if (EmptyListNode * typed = dynamic_cast<EmptyListNode*>(node))
            return NULL;
        throw(EWrongType, *node, StringListNode::generateTypeName(identifierList));
    }
    else
        return NULL;
}

StringNode * AdventureStructure::ListNode::tryGetStringNode(std::string name, StringNode::Type type) const
{
    if (BaseNode * node = tryGet(name))
    {
        if (StringNode * typed = dynamic_cast<StringNode*>(node))
            return typed;
        throw(EWrongType, *node, StringNode::generateTypeName(type));
    }
    else
        return NULL;
}

BaseNode & ListNode::get(std::string name) const
{
    if (BaseNode * result = tryGet(name))
        return *result;
    throw(ENodeNotFound, *this, name);
}

ListNode & AdventureStructure::ListNode::getListNode(std::string name) const
{
    if (ListNode * result = tryGetListNode(name))
        return *result;
    throw(ENodeNotFound, *this, name);
}

StringListNode & AdventureStructure::ListNode::getStringListNode(std::string name, bool identifierList) const
{
    if (StringListNode * result = tryGetStringListNode(name, identifierList))
        return *result;
    throw(ENodeNotFound, *this, name);
}

StringNode & AdventureStructure::ListNode::getStringNode(std::string name, StringNode::Type type) const
{    
    if (StringNode * result = tryGetStringNode(name, type))
        return *result;
    throw(ENodeNotFound, *this, name);
}

std::string AdventureStructure::ListNode::getString(std::string name, StringNode::Type type) const
{
    StringNode & node = getStringNode(name, type);
    std::string result = node.getValue();
    return result;
}

bool AdventureStructure::ListNode::getBoolean(std::string name, bool required, bool defaultValue) const
{
    if (StringNode * node = tryGetStringNode(name, StringNode::stIdent))
    {
        std::string value = node->getValue();
        if (value == "true")
            return true;
        if (value == "false")
            return false;
        throw(EInvalidBoolValue, *node, value);
    }
    else if (required)
        throw(ENodeNotFound, *this, name);
    return defaultValue;
}

stringlist AdventureStructure::ListNode::getStringList(std::string name, bool identList, bool oneRequired) const
{
    stringlist result;          
    if (StringListNode * node = tryGetStringListNode(name, identList))
    {
        for (std::string value : *node)
            result.push_back(value);
    }
    else if (oneRequired)
    {
        throw(EEmptyOrMissing, *this, name);
    }
    return result;
}

AliasList AdventureStructure::ListNode::getAliases() const
{
    AliasList result;
    for (std::string value : getStringList("Aliases", false))
        result.add(value, false); 

    for (std::string value : getStringList("PluralAliases", false))
        result.add(value, true);

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
    markAsUsed();
    for (BaseNode & node : nodes)
        node.markAsUsed();
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

void AdventureStructure::ListNode::markChildAsUsed(std::string name) const
{
    if (BaseNode * node = tryGet(name))
    {
        if (ListNode * list = dynamic_cast<ListNode*>(node))
            list->markAsUsedRecursive();
        else
            node->markAsUsed();
    }
}

void AdventureStructure::ListNode::markAsUsedRecursive()
{
    markAsUsed();
    for (BaseNode & node : nodes)
    {
        if (ListNode * list = dynamic_cast<ListNode*>(&node))
            list->markAsUsedRecursive();
        else
            node.markAsUsed();
    }
}

ref_vector<BaseNode> AdventureStructure::ListNode::getUnusedNodes() const
{
    ref_vector<BaseNode> result;
    for (BaseNode & node : nodes)
    {
        if (!node.isUsed())
            result.push_back(node);
        else
            if (ListNode * list = dynamic_cast<ListNode*>(&node))
            {
                ref_vector<BaseNode> more = list->getUnusedNodes();
                result.reserve(result.size() + more.size());
                result.insert(result.end(), more.begin(), more.end());
            }
    }
    return result;
}

StringListNode::StringListNode(std::string name, ListNode & parent, bool identifierList, LineInfo keyPos)
    : BaseNode(name, parent, keyPos)
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

std::string & StringListNode::get(size_t index)
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

StringNode::StringNode(std::string name, ListNode & parent, std::string value, Type type, LineInfo keyPos, LineInfo textPos)
    : BaseNode(name, parent, keyPos)
    , textPos(textPos)
    , value(value)
    , type(type)
{
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

const LineInfo & AdventureStructure::StringNode::getTextLineInfo() const
{
    return textPos;
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

AdventureStructure::RootNode::RootNode()
    : ListNode("")
{
}

AdventureStructure::RootNode::RootNode(std::wstring filename)
    : RootNode()
{
    loadFromFile(filename);
}

void RootNode::loadFromString(std::string text)
{
    LineInfo lineinfo;
    LineInfo lastKeyPos;
    LineInfo lastTextPos;

    ListNode * currentParent = this;

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
        lineinfo.line += count(text.cbegin(), text.cend(), '\n');
        size_t lastNewline = text.find_last_of('\n');
        if (lastNewline == std::string::npos)
            lineinfo.col += text.size();
        else
            lineinfo.col = text.size() - lastNewline;
    };
    /*
    auto regex_check = [&](const std::regex& regexString)
    {
        return std::regex_search(text.cbegin() + pos, text.cend(), matches, regexString, std::regex_constants::match_continuous);
    };
    */
    auto quick_check = [&](const std::string& word)
    {
        return !text.compare(lineinfo.pos, word.length(), word);
    };
    /*
    auto regex_quick_check = [&](const std::regex& regexString, const std::string& quickCheckWord)
    {
        return quick_check(quickCheckWord) && 
            std::regex_search(text.cbegin() + pos, text.cend(), matches, regexString, std::regex_constants::match_continuous);
    };
    */
    auto check_code = [&](std::string & code)
    {
        if (!quick_check(codeStart))
            return false;

        size_t i = lineinfo.pos + codeStart.size();
        size_t j;

        lastTextPos = lineinfo;
        lastTextPos.pos += codeStart.size();
        lastTextPos.col += codeStart.size();

        bool startMarked = false;

        while (i < text.size())
        {
            if (!startMarked)
            {
                lastTextPos.pos++;
                if (text[i] == ' ' || text[i] == '\t' || text[i] == '\r')
                {
                    lastTextPos.col++;
                }
                else if (text[i] == '\n')
                {
                    lastTextPos.col = 1;
                    lastTextPos.line++;
                }
                else
                    startMarked = true;
            }

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
        size_t i = lineinfo.pos;
        outIdent = "";
        while (text[i] >= 'a' && text[i] <= 'z' ||
            text[i] >= 'A' && text[i] <= 'Z' ||
            text[i] >= '0' && text[i] <= '9' ||
            text[i] == '_')
        {
            outIdent += text[i++];
        }
        return i != lineinfo.pos;
    };

    auto check_ident_length = [&](size_t & length)
    {
        size_t i = lineinfo.pos;
        while (text[i] >= 'a' && text[i] <= 'z' ||
            text[i] >= 'A' && text[i] <= 'Z' ||
            text[i] >= '0' && text[i] <= '9' ||
            text[i] == '_')
        {
            i++;
        }
        length = i - lineinfo.pos;
        return length != 0;
    };

    auto parseString = [&](std::string & result)
    {
        lineinfo.pos++;
        lineinfo.col++;
        while (lineinfo.pos < text.size() && text[lineinfo.pos] != '"')
        {
            if (text[lineinfo.pos] == '\n' || text[lineinfo.pos] == '\r')
                throw(EStringParseError, *currentParent, "Unexpected end of line in string \"" + result + "\"");

            if (text[lineinfo.pos] == '\\')
            {
                if (lineinfo.pos + 1 < text.size())
                {
                    switch (text[lineinfo.pos + 1])
                    {
                    case '"':
                        result += '"';
                        break;
                    case '\\':
                        result += '\\';
                        break;
                    default:
                        throw(EStringParseError, *currentParent, std::string("Only \\\" and \\\\ escaping is supported, got \\") + 
                            text[lineinfo.pos + 1] + " in string \"" + result + "\"");
                    }
                }
                else
                    throw(EStringParseError, *currentParent, "Unexpected end of file after \\ in string \"" + result + "\"");
                lineinfo.pos++;
                lineinfo.col++;
            }
            else
                result += text[lineinfo.pos];
            lineinfo.pos++;
            lineinfo.col++;
        }
        if (lineinfo.pos == text.size())
            throw(EStringParseError, *currentParent, "Unexpected end of file after \\ in string \"" + result + "\"");
            
        lineinfo.pos++;
        lineinfo.col++;
    };

    auto skipWhitespacesAndComments = [&]()
    {
        bool more = true;
        bool skippedSome = false;

        while (more)
        {
            // skip whitespaces

            if (text[lineinfo.pos] == ' ' ||
                text[lineinfo.pos] == '\n' ||
                text[lineinfo.pos] == '\t' ||
                text[lineinfo.pos] == '\r')
            {
                do
                {
                    if (text[lineinfo.pos] == '\n')
                    {
                        lineinfo.line++;
                        lineinfo.col = 1;
                    }
                    else
                    {
                        lineinfo.col++;
                    }
                    lineinfo.pos++;
                } while (text[lineinfo.pos] == ' ' ||
                         text[lineinfo.pos] == '\n' ||
                         text[lineinfo.pos] == '\t' ||
                         text[lineinfo.pos] == '\r');

                skippedSome = true;
                continue;
            }
            
            // ignore // comment
            if (quick_check(lineCommentQuick))
            {
                lineinfo.pos = text.find('\n', lineinfo.pos + 2);
                lineinfo.line++;
                lineinfo.col = 1;
                skippedSome = true;
                continue;
            }

            // ignore { comment }
            if (quick_check(multilineCommentQuick))
            {
                size_t oldPos = lineinfo.pos;
                lineinfo.pos = text.find('}', lineinfo.pos + 1);
                if (lineinfo.pos == std::string::npos)
                    lineinfo.pos = text.size();
                else
                    lineinfo.pos++;
                updateLine(text.substr(oldPos, lineinfo.pos - oldPos));
                skippedSome = true;
                continue;
            }
            
            more = false;
        } 
        return skippedSome;
    };

    while (lineinfo.pos < text.size())
    {
        if (skipWhitespacesAndComments())
            continue;

        lastKeyPos = lineinfo;

        // end
        if (quick_check(end))
        {
            if (currentParent->hasParent())
                currentParent = &currentParent->getParent();
            else
                throw(EAdventureStructureParse, *currentParent, "No matching \"IDENTIFIER:\" for \"end\"");

            lineinfo.pos += end.size();
            lineinfo.col += end.size();
            continue;
        }

        // IDENTIFIER
        std::string key;
        if (check_ident(key))
        {
            BaseNode * duplicate = currentParent->tryGet(key);
            
            lineinfo.pos += key.size();
            lineinfo.col += key.size();

            skipWhitespacesAndComments();

            // IDENTIFIER =
            if (text[lineinfo.pos] == '=')
            {
                if (duplicate)
                    throw(ENodeExistsAlready, *currentParent, *duplicate);

                lineinfo.pos++;
                lineinfo.col++;
            
                skipWhitespacesAndComments();

                std::string code;
                if (check_code(code))
                {
                    // IDENFITIER = \/CODE text /\END
                    lineinfo.pos += code.size() + codeStart.size() + codeEnd.size();
                    updateLine(code);
                    size_t first = code.find_first_not_of(" \n\r\t");
                    if (first != std::string::npos)
                        code = code.substr(first, code.find_last_not_of(" \n\r\t") - first + 1);
                    new StringNode(key, *currentParent, code, StringNode::stCode, lastKeyPos, lastTextPos);
                    
                    continue;
                }
                if (text[lineinfo.pos] == '"')
                {
                    // IDENTIFIER = "text"

                    lastTextPos = lineinfo;

                    std::string result;
                    parseString(result);

                    new StringNode(key, *currentParent, result, StringNode::stString, lastKeyPos, lastTextPos);
                    continue;
                }
                std::string value;
                if (check_ident(value))
                {
                    lastTextPos = lineinfo;

                    lineinfo.pos += value.size();
                    lineinfo.col += value.size();
                    new StringNode(key, *currentParent, value, StringNode::stIdent, lastKeyPos, lastTextPos);
                    continue;
                }

                throw(EAdventureStructureParse, *currentParent, "Expected string, identifier or code-block for assignment for key \"" + key + "\"");
            }

            // IDENTIFIER:
            if (text[lineinfo.pos] == ':')
            {      
                lineinfo.pos++;
                lineinfo.col++;

                skipWhitespacesAndComments();

                if (text[lineinfo.pos] == '"')
                {
                    if (duplicate)
                        throw(ENodeExistsAlready, *currentParent, *duplicate);
                    
                    // IDENTIFIER: "test" "hallo" END  
                    StringListNode& node = *new StringListNode(key, *currentParent, false, lastKeyPos);
                    do
                    {
                        // check if we really have another string
                        if (text[lineinfo.pos] != '"')
                            throw(EAdventureStructureParse, node, "Expected next string or \"end\"");
                        std::string result;
                        parseString(result);
                        node.add(result);
                        // skip whitespaces
                        if (!skipWhitespacesAndComments())
                            throw(EAdventureStructureParse, node, "Expected space after end of string \"" + result + "\"");
                    } while (!quick_check(end));
                    lineinfo.pos += end.size();
                    lineinfo.col += end.size();
                    continue;
                }
                if (quick_check(end))
                {
                    // empty, can't define type
                    lineinfo.pos += end.size();
                    lineinfo.col += end.size();
                    new EmptyListNode(key, *currentParent, lastKeyPos);
                    continue;
                }
                size_t identLength;
                if (check_ident_length(identLength))
                {
                    LineInfo saveinfo = lineinfo;

                    lineinfo.pos += identLength;
                    lineinfo.col += identLength;

                    skipWhitespacesAndComments();
                    if (text[lineinfo.pos] == ':' || text[lineinfo.pos] == '=')
                    {
                        // IDENTIFIER: ID1: END ID2: END END 
                        // or
                        // IDENTIFIER: ID1 = "test" END  
                        if (duplicate)
                        {
                            try
                            {
                                currentParent = &dynamic_cast<ListNode&>(*duplicate);
                            }
                            catch (std::bad_cast)
                            {
                                throw(EAdventureStructureParse, *currentParent, "Cannot merge Identifier \"" + key + "\", since they are not of the same type!");
                            }
                        }
                        else
                        {
                            currentParent = new ListNode(key, *currentParent, lastKeyPos);
                        }
                        lineinfo = saveinfo;
                        continue;
                    }

                    lineinfo = saveinfo;

                    // IDENTIFIER: test hallo END      
                    StringListNode & node = *new StringListNode(key, *currentParent, true, lastKeyPos);
                    std::string ident;
                    while (check_ident(ident))
                    {
                        lineinfo.pos += ident.size();
                        lineinfo.col += ident.size();
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

            throw(EAdventureStructureParse, *currentParent, "Identifier \"" + key + "\" not followed by \":\" or \"=\"");
        }

        size_t begin = text.find_last_of('\n', lineinfo.pos);
        size_t end = text.find_first_of('\n', lineinfo.pos);
        if (begin == std::string::npos)
            begin = 0;
        else
            begin++;
        throw(EAdventureStructureParse, *currentParent, "Unknown error at \"" + text.substr(begin, end - begin) + "\"");   
    }

    if (currentParent->hasParent())
    {
        if (currentParent->getDepth() == 1)
            throw(EAdventureStructureParse, *currentParent, "Missing \"end\" at end of file");
        else
            throw(EAdventureStructureParse, *currentParent, "Missing " + std::to_string(currentParent->getDepth()) + " \"end\"s at end of file");
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

EmptyListNode::EmptyListNode(std::string name, ListNode & parent, LineInfo keyPos)
    : BaseNode(name, parent, keyPos)
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

AdventureStructure::EAdventureStructure::EAdventureStructure(const BaseNode & node, const std::string & msg)
    : Exception(msg)
    , node(node)

{
}

const BaseNode & AdventureStructure::EAdventureStructure::getNode() const
{
    return node;
}

AdventureStructure::EAdventureStructureParse::EAdventureStructureParse(const BaseNode & node, const std::string & msg)
    : EAdventureStructure(node, msg)
{
}

AdventureStructure::ENodeNotFound::ENodeNotFound(const ListNode & node, const std::string & name)
    : EAdventureStructure(node, "The node \"" + name + "\" could not be found")
{
}

AdventureStructure::EListEmpty::EListEmpty(const ListNode & node, const std::string & name)
    : EAdventureStructure(node, "The list \"" + name + "\" is empty")
{
}

AdventureStructure::EWrongType::EWrongType(const BaseNode & node, const std::string & expected)
    : EAdventureStructure(node, "The node \"" + node.getName() + "\" should be of type \"" + expected + "\", but is \"" + node.getTypeName() + "\"")
{
}

AdventureStructure::ENodeExistsAlready::ENodeExistsAlready(const ListNode & base, const BaseNode & node)
    : EAdventureStructure(base, "Node \"" + node.getName() + "\" is already a subnode of \"" + base.getName() + "\"")
{
}

AdventureStructure::EInvalidBoolValue::EInvalidBoolValue(const StringNode & node, const std::string & value)
    : EAdventureStructure(node, "\"" + value + "\" is not a valid boolean value [true/false]")
{
}

AdventureStructure::ERootHasNoParent::ERootHasNoParent(const BaseNode & node)
    : EAdventureStructure(node, "Cannot get the parent of the root node")
{
}

AdventureStructure::EStringParseError::EStringParseError(const BaseNode & node, const std::string & msg)
    : EAdventureStructureParse(node, msg)
{
}

AdventureStructure::ENodeDoesNotExist::ENodeDoesNotExist(const ListNode & base, const BaseNode & node)
    : EAdventureStructure(base, "Node \"" + node.getName() + "\" is not a subnode of \"" + base.getName() + "\"")
{
}

AdventureStructure::EEmptyOrMissing::EEmptyOrMissing(const ListNode & node, const std::string & name)
    : EAdventureStructure(node, "List node \"" + node.getName() + "\" requires at least one entry")
{
}

AdventureStructure::LineInfo::LineInfo()
    : line(1)
    , col(1)
    , pos(0)
{
}
