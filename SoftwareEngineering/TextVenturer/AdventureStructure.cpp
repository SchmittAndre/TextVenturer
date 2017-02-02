#include "stdafx.h"

#include "AdventureStructure.h"

using namespace AdventureStructure;

BaseNode::BaseNode(string name, ListNode * parent)
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

string BaseNode::getName() const
{
    return name;
}

ListNode * AdventureStructure::BaseNode::getParent()
{
    return parent;
}

bool ListNode::del(BaseNode * node)
{
    vector<BaseNode*>::iterator pos = find(nodes.begin(), nodes.end(), node);
    if (pos == nodes.end())
        return false;
    nodes.erase(pos);
    return true;
}

BaseNode * ListNode::operator[](size_t index) const
{
    return nodes[index];
}

AdventureStructure::ListNode::ListNode(string name, ListNode * parent)
    : BaseNode(name, parent)
{
}

AdventureStructure::ListNode::~ListNode()
{
    for (int i = nodes.size() - 1; i >= 0; i--)
        delete nodes[i];
}

bool ListNode::add(BaseNode * node)
{
    if ((*this)[node->getName()])
        return false;
    nodes.push_back(node);
    return true;
}

BaseNode * ListNode::operator[](string name) const
{
    for (vector<BaseNode*>::const_iterator current = nodes.begin(); current != nodes.end(); current++)
        if ((*current)->getName() == name)
            return *current;
    return NULL;
}

AdventureStructure::StringListNode::StringListNode(string name, ListNode * parent, bool identifierList)
    : BaseNode(name, parent)
{
    this->identifierList = identifierList;
}

void StringListNode::add(string name)
{
    items.push_back(name);
}

void StringListNode::del(size_t index)
{
    items.erase(items.begin() + index);
}

bool AdventureStructure::StringListNode::isIdentifierList() const
{
    return identifierList;
}

string& StringListNode::operator[](size_t index)
{
    return items[index];
}

size_t AdventureStructure::StringListNode::getCount()
{
    return items.size();
}

AdventureStructure::StringNode::StringNode(string name, ListNode * parent, string value, Type type)
    : BaseNode(name, parent)
{
    this->value = value;
    this->type = type;
}

string StringNode::getValue() const
{
    return value;
}

void StringNode::setValue(string value)
{
    this->value = value;
}

StringNode::Type StringNode::getType() const
{
    return type;
}

AdventureStructure::RootNode::RootNode()
    : ListNode("root", NULL)
{
}

bool RootNode::loadFromString(string text)
{
    int linenumber = 1;
    int offset = 1;
    size_t pos = 0;

    ListNode* currentParent = this;

    const string ident = "[a-zA-Z0-9_]+";
    const string spaces = "[ \n\r\t]*";
    const string spaces1 = "[ \n\r\t]+";
    const string any = "[^]*?";

    smatch matches;

    auto error = [&linenumber, &offset](string msg, string additional = "")
    {
        if (additional != "")
            additional = "\n> " + additional;
        ErrorDialog(msg + " at line " + to_string(linenumber) + " column " + to_string(offset) + additional);
    };

    auto updateLine = [&linenumber, &offset](string text)
    {
        linenumber += count(text.cbegin(), text.cend(), '\n');
        size_t lastNewline = text.find_last_of('\n');
        if (lastNewline == string::npos)
            offset += text.size();
        else
            offset = text.size() - lastNewline;
    };

    string lstr;
    auto regex_check = [&matches, &text, &pos, &lstr](string regexString)
    {
        if (lstr.length() < regexString.length())
            lstr = regexString;
        return regex_search(text.cbegin() + pos, text.cend(), matches, regex(regexString), regex_constants::match_continuous);
    };

    auto parseString = [&error, &offset, &pos, &text](string &result)
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
            else if (text[pos] == '\\')
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
                        error("Only \\\" and \\\\ escaping is supported", result);
                    }
                }
                else
                {
                    error("Unexpected end of file after \\", result);
                }
            }
            else
                result += text[pos];
            pos++;
            offset++;
        }
        if (pos == text.size())
        {
            error("Unexpected end of file in string", result);
            return false;
        }
        pos++;
        offset++;
    };

    auto skipWhitespacesAndComments = [&regex_check, &pos, &linenumber, &offset, &matches, &spaces1, &updateLine]()
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

            // ignore comments
            if (regex_check("//.*\\n?"))
            {
                pos += matches[0].length();
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
        skipWhitespacesAndComments();

        // IDENTIFIER = 
        if (regex_check("(" + ident + ")(" + spaces + "=" + spaces + ")"))
        {
            pos += matches[0].length();
            offset += matches[1].length();            
            updateLine(matches[2]);

            skipWhitespacesAndComments();

            string name = matches[1];     
            if (regex_check("CODE" + spaces1 + "(" + any + ")" + spaces1 + "END"))
            {
                // IDENFITIER = CODE text END
                pos += matches[0].length();
                string code = matches[1];
                new StringNode(name, currentParent, code, StringNode::stCode);
                updateLine(matches[0]);
                continue;
            }
            else if (text[pos] == '"')
            {
                // IDENTIFIER = "text"
                string result;
                if (!parseString(result))
                    return false;
                
                new StringNode(name, currentParent, result, StringNode::stString);
                continue;
            }
            else if (regex_check(ident))
            {
                pos += matches[0].length();
                offset += matches[0].length();
                new StringNode(name, currentParent, matches[0], StringNode::stIdent);
                continue;
            }
            else
            {
                error("Expected string, identifier or code-block for assignment", name);
                return false;
            }            
        }

        // IDENTIFIER:
        if (regex_check("(" + ident + "):"))
        {
            pos += matches[0].length();
            offset += matches[0].length();
            string name = matches[1];

            skipWhitespacesAndComments();
            
            if (text[pos] == '"')
            {
                // IDENTIFIER: "test" "hallo" END  
                StringListNode* node = new StringListNode(name, currentParent, false);
                while (!regex_check("end"))
                {
                    string result;
                    if (!parseString(result))
                        return false;  
                    node->add(result);
                    // skip whitespaces
                    if (!skipWhitespacesAndComments())
                    {
                        error("Expected space after end of string", result);
                        return false;
                    }
                }
                pos += matches[0].length();
                offset += matches[0].length();
                continue;
            }
            else if (regex_check(ident + spaces + ":"))
            {
                // IDENTIFIER: ID1: END ID2: END END   
                currentParent = new ListNode(name, currentParent);
                continue;
            }
            else if (regex_check(ident + spaces + "="))
            {
                // IDENTIFIER: ID1 = "test" END   
                currentParent = new ListNode(name, currentParent);
                continue;
            }
            else if (regex_check("end"))
            {
                // empty lists are node lists
                pos += matches[0].length();
                offset += matches[0].length();
                new ListNode(name, currentParent);
                continue;
            }
            else
            {
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
                        error("Expected space after end of Identifier-List " + name);
                        return false;
                    } 
                }
                if (!matches.size())
                {
                    error("Error scanning Identifier-List " + name);
                    return false;
                }
                continue;
            }
        }

        if (regex_check("end"))
        {            
            pos += matches[0].length();
            offset += matches[0].length();

            currentParent = currentParent->getParent();
            if (!currentParent)
            {
                error("No matching \"XXX:\" for end");
                return false;
            }
            continue;
        }

        size_t begin = text.find_last_of('\n', pos);
        size_t end = text.find_first_of('\n', pos);
        if (begin == string::npos)
            begin = 0;
        else
            begin++;
        error("Unknown error", text.substr(begin, end - begin));
        return false;
    }

    if (currentParent->getParent())
    {
        ErrorDialog("Missing end at end of file");
        return false;
    }

    return true;
}

bool RootNode::loadFromFile(string filename)
{
    ifstream file(filename);
    if (!file.good())
    {
        ErrorDialog("Could not find file \"" + filename + "\"");
        return false;
    }
    stringstream stream;
    stream << file.rdbuf();
    return loadFromString(stream.str());
}
