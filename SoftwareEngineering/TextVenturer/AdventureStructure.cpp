#include "stdafx.h"

#include "AdventureStructure.h"

using namespace AdventureStructure;

BaseNode::BaseNode(std::string name, ListNode * parent)
{
    this->name = name;
    this->parent = parent;
    if (parent)
        parent->add(this);
}

BaseNode::~BaseNode()
{
    if (parent)
        parent->del(this);
}

std::string BaseNode::getName() const
{
    return name;
}

ListNode * BaseNode::getParent()
{
    return parent;
}

bool AdventureStructure::BaseNode::named(std::string name) const
{
    return this->name == name;
}

std::string AdventureStructure::BaseNode::getFullPath()
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

BaseNode::operator EmptyListNode*()
{
    return dynamic_cast<EmptyListNode*>(this);
}

BaseNode::operator ListNode*()
{
    return dynamic_cast<ListNode*>(this);
}

BaseNode::operator StringNode*()
{
    return dynamic_cast<StringNode*>(this);
}

BaseNode::operator StringListNode*()
{
    return dynamic_cast<StringListNode*>(this);
}

AdventureStructure::BaseNode::operator RootNode*()
{
    return dynamic_cast<RootNode*>(this);
}

std::string AdventureStructure::BaseNode::getTypeName()
{
    return "BaseNode";
}

bool ListNode::del(BaseNode * node)
{
  std::vector<BaseNode*>::iterator pos = find(nodes.begin(), nodes.end(), node);
    if (pos == nodes.end())
        return false;
    nodes.erase(pos);
    return true;
}

BaseNode * ListNode::get(size_t index) const
{
    return nodes[index];
}

ListNode::ListNode(std::string name, ListNode * parent)
    : BaseNode(name, parent)
{
}

ListNode::~ListNode()
{
    for (int i = nodes.size() - 1; i >= 0; i--)
        delete nodes[i];
}

bool ListNode::add(BaseNode * node)
{
    if (this->get(node->getName()))
        return false;
    nodes.push_back(node);
    return true;
}

BaseNode * ListNode::get(std::string name) const
{
    for (std::vector<BaseNode*>::const_iterator current = nodes.begin(); current != nodes.end(); current++)
        if ((*current)->getName() == name)
            return *current;
    return NULL;
}

bool AdventureStructure::ListNode::empty() const
{
    return nodes.empty();
}

size_t AdventureStructure::ListNode::getCount() const
{
    return nodes.size();
}

std::vector<BaseNode*>::iterator ListNode::begin()
{
    return nodes.begin();
}

std::vector<BaseNode*>::iterator ListNode::end()
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


strings::iterator StringListNode::begin()
{
    return items.begin();
}


strings::iterator StringListNode::end()
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
    int linenumber = 1;
    int offset = 1;
    size_t pos = 0;

    ListNode* currentParent = this;

    const std::string ident = "[a-zA-Z0-9_]+";
    const std::string spaces = "[ \n\r\t]*";
    const std::string spaces1 = "[ \n\r\t]+";
    const std::string any = "[^]*?";

    std::smatch matches;

    auto error = [&linenumber, &offset, &currentParent](std::string msg, std::string additional = "", bool showPath = true)
    {
        if (additional != "")
            additional = "\n> " + additional;
        if (showPath)
            additional += "\n\nPath: " + currentParent->getFullPath();
        ErrorDialog("Script Parsing", msg + " at line " + std::to_string(linenumber) + " column " + std::to_string(offset) + additional);
    };

    auto updateLine = [&linenumber, &offset](std::string text)
    {
        linenumber += count(text.cbegin(), text.cend(), '\n');
        size_t lastNewline = text.find_last_of('\n');
        if (lastNewline == std::string::npos)
            offset += text.size();
        else
            offset = text.size() - lastNewline;
    };

    auto regex_check = [&matches, &text, &pos](std::string regexString)
    {
        return std::regex_search(text.cbegin() + pos, text.cend(), matches, std::regex(regexString), std::regex_constants::match_continuous);
    };

    auto parseString = [&error, &offset, &pos, &text](std::string &result)
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

    auto skipWhitespacesAndComments = [&regex_check, &pos, &linenumber, &offset, &matches, &spaces1, &updateLine, &any]()
    {
        bool more = true;
        bool skippedSome = false;
        while (more)
        {
            // skip whitespaces
            if (regex_check(spaces1))
            {
                pos += matches[0].length();
                updateLine(matches[0]);
                skippedSome = true;
                continue;
            }

            // ignore // comment
            if (regex_check("//.*\\n?"))
            {
                pos += matches[0].length();
                linenumber++;
                offset = 1;
                skippedSome = true;
                continue;
            }

            // ignore { comment }
            if (regex_check("\\{" + any + "\\}"))
            {
                pos += matches[0].length();
                updateLine(matches[0]);                
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
        if (regex_check("end"))
        {
            currentParent = currentParent->getParent();
            if (!currentParent)
            {
                error("No matching \"IDENTIFIER:\" for \"end\"", "", false);
                return false;
            }

            pos += matches[0].length();
            offset += matches[0].length();
            continue;
        }

        // IDENTIFIER
        if (regex_check(ident))
        {
            std::string name = matches[0];
            if (currentParent->get(name))
            {
                error("Duplicate Identifier \"" + name + "\"");
                return false;
            }

            pos += matches[0].length();
            offset += matches[0].length();

            skipWhitespacesAndComments();

            // IDENTIFIER =
            if (text[pos] == '=')
            {
                pos++;
                offset++;
            
                skipWhitespacesAndComments();

                if (regex_check("\\\\/CODE" + spaces1 + "(" + any + ")" + spaces1 + "/\\\\END"))
                {
                    // IDENFITIER = \/CODE text /\END
                    pos += matches[0].length();
                    std::string code = matches[1];
                    new StringNode(name, currentParent, code, StringNode::stCode);
                    updateLine(matches[0]);
                    continue;
                }
                if (text[pos] == '"')
                {
                    // IDENTIFIER = "text"
                    std::string result;
                    if (!parseString(result))
                        return false;

                    new StringNode(name, currentParent, result, StringNode::stString);
                    continue;
                }
                if (regex_check(ident))
                {
                    pos += matches[0].length();
                    offset += matches[0].length();
                    new StringNode(name, currentParent, matches[0], StringNode::stIdent);
                    continue;
                }

                error("Expected string, identifier or code-block for assignment", name);
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
                    // IDENTIFIER: "test" "hallo" END  
                    StringListNode* node = new StringListNode(name, currentParent, false);
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
                    } while (!regex_check("end"));
                    pos += matches[0].length();
                    offset += matches[0].length();
                    continue;
                }
                if (regex_check("end"))
                {
                    // empty, can't define type
                    pos += matches[0].length();
                    offset += matches[0].length();
                    new EmptyListNode(name, currentParent);
                    continue;
                }
                if (regex_check(ident))
                {
                    size_t savepos = pos;

                    pos += matches[0].length();

                    skipWhitespacesAndComments();
                    if (text[pos] == ':' || text[pos] == '=')
                    {
                        // IDENTIFIER: ID1: END ID2: END END 
                        // or
                        // IDENTIFIER: ID1 = "test" END   
                        currentParent = new ListNode(name, currentParent);
                        pos = savepos;
                        continue;
                    }

                    pos = savepos;

                    // IDENTIFIER: test hallo END      
                    StringListNode* node = new StringListNode(name, currentParent, true);
                    while (regex_check(ident))
                    {
                        pos += matches[0].length();
                        offset += matches[0].length();
                        if (matches[0] == "end")
                            break;
                        node->add(matches[0]);

                        if (!skipWhitespacesAndComments())
                        {
                            error("Expected space in Identifier-List \"" + name + "\"");
                            return false;
                        }
                    }
                    continue;
                }
                if (!matches.size())
                {
                    error("Error scanning Identifier-List \"" + name + "\"");
                    return false;
                }
                continue;
            }

            error("Identifier \"" + name + "\" not followed by \":\" or \"=\"");
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

bool RootNode::loadFromFile(std::string filename)
{
    std::ifstream file(filename);
    if (!file.good())
    {
        ErrorDialog("Could not find file \"" + filename + "\"");
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
