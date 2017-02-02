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

AdventureStructure::StringListNode::StringListNode(string name, ListNode * parent, bool identifier)
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

AdventureStructure::StringNode::StringNode(string name, ListNode * parent, string value, bool code)
    : BaseNode(name, parent)
{
    this->value = value;
    this->code = code;
}

string StringNode::getValue() const
{
    return value;
}

void StringNode::setValue(string value)
{
    this->value = value;
}

bool StringNode::isCode() const
{
    return code;
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

    const string ident = "[a-zA-Z0-9]+";
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

    auto regex_check = [&matches, &text](size_t offset, string regexString)
    {
        return regex_search(text.cbegin() + offset, text.cend(), matches, regex(regexString), regex_constants::match_continuous);
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

    while (pos < text.size())
    {
        // skip whitespaces
        if (regex_check(pos, spaces1))
        {
            pos += matches[0].length();
            updateLine(matches[0]);
            continue;
        }

        // ignore comments
        if (regex_check(pos, "//.*\\n?"))
        {
            pos += matches[0].length();
            linenumber++;
            offset = 1;
            continue;
        }

        // IDENTIFIER = 
        if (regex_check(pos, "(" + ident + ")(" + spaces + "=" + spaces + ")"))
        {
            pos += matches[0].length();
            offset += matches[1].length();            
            updateLine(matches[2]);

            string name = matches[1];     
            StringNode* node = NULL;
            if (regex_check(pos, "CODE" + spaces1 + "(" + any + ")" + spaces1 + "END"))
            {
                // IDENFITIER = CODE text END
                pos += matches[0].length();
                string code = matches[1];
                node = new StringNode(name, currentParent, code, true);
                updateLine(matches[0]);
                continue;
            }
            else if (text[pos] == '"')
            {
                // IDENTIFIER = "text"
                string result;
                if (!parseString(result))
                    return false;
                
                node = new StringNode(name, currentParent, result, false);
                continue;
            }
            else
            {
                error("Expected string or code-block", name);
                return false;
            }            
        }

        // IDENTIFIER:
        if (regex_check(pos, "(" + ident + ")(:" + spaces + ")"))
        {
            pos += matches[0].length();
            offset += matches[1].length();
            updateLine(matches[2]);

            string name = matches[1];
            if (text[pos] == '"')
            {
                // IDENTIFIER: "test" "hallo" END  
                StringListNode* node = new StringListNode(name, currentParent, false);
                while (text[pos] == '"')
                {
                    string result;
                    if (!parseString(result))
                        return false;  
                    node->add(result);
                    // skip whitespaces
                    if (regex_check(pos, spaces1))
                    {
                        pos += matches[0].length();
                        updateLine(matches[0]);
                    }
                    else
                    {
                        error("Expected space after end of string", result);
                    }
                }
                return false;
            }
            else if (regex_check(pos, ident))
            {
                if (regex_check(pos, ident + spaces1 + ident))
                {
                    // IDENTIFIER: test hallo END      
                    error("IdentifierList not supported yet");
                    return false;
                }
                else
                {
                    // IDENTIFIER: ID1: END ID2: END END   
                    currentParent = new ListNode(name, currentParent);
                    continue;
                }
            }
            else
            {
                error("Could not parse list item", name);
                return false; 
            }
        }

        if (regex_check(pos, "end"))
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
        /*    
            // string list
            IDENTIFIER:
                "element 1"
                "other element"
            END

            // node list
            IDENTIFIER:
                // nodes or empty
            END
        */
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
