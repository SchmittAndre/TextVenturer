#include "stdafx.h"

#include "AdventureStructure.h"

using namespace AdventureStructure;

BaseNode::BaseNode(std::string name, ListNode & parent)
{
    this->name = name;
    this->parent = parent;
    parent.add(*this);
}

BaseNode::~BaseNode()
{
    if (parent)
        parent->get().del(*this);
}

std::string BaseNode::getName() const
{
    return name;
}

ListNode & BaseNode::getParent()
{
    return parent.value();
}

std::string AdventureStructure::BaseNode::getFullPath()
{
    if (parent)
        return parent->get().getFullPath() + "/" + name;
    else
        return name;
}

size_t AdventureStructure::BaseNode::getDepth()
{
    if (parent)
        return parent->get().getDepth() + 1;
    else
        return 0;
}

std::string AdventureStructure::BaseNode::getTypeName()
{
    return "BaseNode";
}

void ListNode::del(BaseNode & node)
{
    std::vector<BaseNode*>::iterator pos = find(nodes.begin(), nodes.end(), node);
    if (pos == nodes.end())
        throw(ETodo);
    nodes.erase(pos);
}

BaseNode & ListNode::get(size_t index) const
{
    return *nodes[index];
}

bool AdventureStructure::ListNode::hasChild(std::string name) const
{
    for (auto pos = nodes.begin(); pos != nodes.end(); pos++)
        if ((*pos)->getName() == name)
            return true;
    return false;
}

ListNode::ListNode(std::string name, ListNode & parent)
    : BaseNode(name, parent)
{
}

ListNode::~ListNode()
{
    for (LONG_PTR i = (LONG_PTR)nodes.size() - 1; i >= 0; i--)
        delete &nodes[i];
}

void ListNode::add(BaseNode & node)
{
    if (hasChild(node.getName()))
        throw(ETodo);
    nodes.push_back(&node);
}

BaseNode & ListNode::get(std::string name) const
{
    for (std::vector<BaseNode*>::const_iterator current = nodes.begin(); current != nodes.end(); current++)
        if ((*current)->getName() == name)
            return **current;
    throw(ETodo);
}

bool AdventureStructure::ListNode::empty() const
{
    return nodes.empty();
}

size_t AdventureStructure::ListNode::getCount() const
{
    return nodes.size();
}

std::vector<BaseNode&>::iterator ListNode::begin()
{
    return nodes.begin();
}

std::vector<BaseNode&>::iterator ListNode::end()
{
    return nodes.end();
}

std::string AdventureStructure::ListNode::getTypeName()
{
    return "NodeList";
}

std::string AdventureStructure::ListNode::getContentName()
{
    return "Nodes";
}

StringListNode::StringListNode(std::string name, ListNode * parent, bool identifierList)
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

std::string AdventureStructure::StringListNode::getTypeName(bool identList)
{
    return identList ? "IdentList" : "StringList";
}

std::string AdventureStructure::StringListNode::getContentName(bool identList)
{
    return identList ? "Identifiers" : "Strings";
}

StringNode::StringNode(std::string name, ListNode * parent, std::string value, Type type)
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

std::string AdventureStructure::StringNode::getTypeName(Type type)
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
    : ListNode(name, NULL)
{
}

bool RootNode::loadFromString(std::string text)
{
    size_t linenumber = 1;
    size_t offset = 1;
    size_t pos = 0;

    ListNode* currentParent = this;

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
    
    std::smatch matches;

    auto error = [&](std::string msg, std::string additional = "", bool showPath = true)
    {
        if (additional != "")
            additional = "\n> " + additional;
        if (showPath)
            additional += "\n\nPath: " + currentParent->getFullPath();
        ErrorDialog("Script Parsing", msg + " at line " + std::to_string(linenumber) + " column " + std::to_string(offset) + additional);
    };

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
            {
                error("Unexpected end of line in string", result);
                return false;
            }
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
                        error(std::string("Only \\\" and \\\\ escaping is supported, got \\") + text[pos + 1], result);
                        return false;
                    }
                }
                else
                {
                    error("Unexpected end of file after \\", result, false);
                    return false;
                }
                pos++;
                offset++;
            }
            else
                result += text[pos];
            pos++;
            offset++;
        }
        if (pos == text.size())
        {
            error("Unexpected end of file in string", result, false);
            return false;
        }
        pos++;
        offset++;
        return true;
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
            currentParent = currentParent->getParent();
            if (!currentParent)
            {
                error("No matching \"IDENTIFIER:\" for \"end\"", "", false);
                return false;
            }

            pos += end.size();
            offset += end.size();
            continue;
        }

        // IDENTIFIER
        std::string key;
        BaseNode* duplicate;
        if (check_ident(key))
        {
            duplicate = currentParent->get(key);

            pos += key.size();
            offset += key.size();

            skipWhitespacesAndComments();

            // IDENTIFIER =
            if (text[pos] == '=')
            {
                if (duplicate)
                {
                    error("Duplicate Identifier \"" + key + "\"");
                    return false;
                }

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
                    if (!parseString(result))
                        return false;

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

                error("Expected string, identifier or code-block for assignment", key);
                return false;
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
                    {
                        error("Duplicate Identifier \"" + key + "\"");
                        return false;
                    }
                    
                    // IDENTIFIER: "test" "hallo" END  
                    StringListNode* node = new StringListNode(key, currentParent, false);
                    do
                    {
                        // check if we really have another string
                        if (text[pos] != '"')
                        {
                            error("Expected next string or \"end\"");
                            return false;
                        }
                        std::string result;
                        if (!parseString(result))
                            return false;
                        node->add(result);
                        // skip whitespaces
                        if (!skipWhitespacesAndComments())
                        {
                            error("Expected space after end of string", result);
                            return false;
                        }
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
                        if (ListNode* listDuplicate = dynamic_cast<ListNode*>(duplicate))
                        {
                            currentParent = listDuplicate;
                        }
                        else if (duplicate)
                        {
                            error("Cannot merge Identifier \"" + key + "\", since they are not of the same type!");
                            return false;
                        }
                        else
                        {
                            currentParent = new ListNode(key, currentParent);
                        }
                        pos = savepos;
                        continue;
                    }

                    pos = savepos;
                    linenumber = saveline;

                    // IDENTIFIER: test hallo END      
                    StringListNode* node = new StringListNode(key, currentParent, true);
                    std::string ident;
                    while (check_ident(ident))
                    {
                        pos += ident.size();
                        offset += ident.size();
                        if (ident == "end")
                            break;
                        node->add(ident);

                        if (!skipWhitespacesAndComments())
                        {
                            error("Expected space in Identifier-List \"" + key + "\"");
                            return false;
                        }
                    }
                    continue;
                }
                if (!matches.size())
                {
                    error("Error scanning Identifier-List \"" + key + "\"");
                    return false;
                }
                continue;
            }

            error("Identifier \"" + key + "\" not followed by \":\" or \"=\"");
            return false;
        }

        size_t begin = text.find_last_of('\n', pos);
        size_t end = text.find_first_of('\n', pos);
        if (begin == std::string::npos)
            begin = 0;
        else
            begin++;
        error("Unknown error", text.substr(begin, end - begin));   
        return false;
    }

    if (currentParent->getParent())
    {
        if (currentParent->getDepth() == 1)
            error("Missing \"end\" at end of file");
        else
            error("Missing " + std::to_string(currentParent->getDepth()) + " \"end\"s at end of file");
        return false;
    }

    return true;
}

bool RootNode::loadFromFile(std::wstring filename)
{
    std::ifstream file(filename);
    if (!file.good())
    {
        ErrorDialog(L"Could not find file \"" + filename + L"\"");
        return false;
    }
    std::stringstream stream;
    stream << file.rdbuf();
    return loadFromString(stream.str());
}

std::string AdventureStructure::RootNode::getTypeName()
{
    return "RootNode";
}

EmptyListNode::EmptyListNode(std::string name, ListNode * parent)
    : BaseNode(name, parent)
{
}

std::string AdventureStructure::EmptyListNode::getTypeName()
{
    return "Empty List";
}
