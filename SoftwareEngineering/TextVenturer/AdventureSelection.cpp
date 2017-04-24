#include "stdafx.h"

#include "Adventure.h"
#include "FileFinder.h"
#include "TextDisplay.h"         
#include "DisplayChar.h"
#include "LineInput.h"
#include "ListSelection.h"

#include "AdventureSelection.h"

// NamedAdventure

std::string AdventureSelection::NamedAdventure::getDisplayName()
{          
    std::string afilename;
    afilename.reserve(filename.length());
    for (auto c : filename)
        afilename += static_cast<WORD>(c) < 0xFF ? static_cast<char>(c) : '?';

    size_t end = afilename.rfind('.');
    if (end == std::string::npos)
        return "???"; // not possible

    std::string name = std::string(afilename.begin(), afilename.begin() + end);
    std::string ext = std::string(afilename.begin() + end + 1, afilename.end());
    
    if (ext == "txvs")
        return "<S> " + name;
    if (ext == "txvc")
        return "<C> " + name;
    
    return "<?> " + name; // not possible
}

bool AdventureSelection::NamedAdventure::operator<(const NamedAdventure & other)
{
    return filename < other.filename;
}

AdventureSelection::NamedAdventure::NamedAdventure(std::wstring filename)
{
    this->filename = filename;
    adventure = NULL;
}

// AdventureSelection

void AdventureSelection::unloadAdventures()
{
    for (auto entry : adventures)
        delete entry.adventure;
    adventures.clear();
    advSelection->delAll();
}

void AdventureSelection::loadAdventures()
{
    unloadAdventures();     

    std::vector<std::wstring> paths = {
        L"data\\compiled\\*.txvc",
        L"data\\adventure\\*.txvs"
    };

    std::wstring input;
    for (auto c : searchBar->getInput())
        input += c;

    for (std::wstring path : paths)
        for (auto file : FileFinder(path))
        {
            std::wstring name(file.cFileName);
            
            if (input == L"" || name.find(input) != std::string::npos)
                adventures.push_back(NamedAdventure(file.cFileName));
        }

    std::sort(adventures.begin(), adventures.end());

    for (auto entry : adventures)
        advSelection->add(entry.getDisplayName());
                                                           
    listChanged = true;
}

AdventureSelection::AdventureSelection(Controler* controler)
    : GameDisplayer(controler)
{
    searchBar = NULL;
    advSelection = NULL;

}

AdventureSelection::~AdventureSelection()
{
}

void AdventureSelection::notifyLoad()
{
    GameDisplayer::notifyLoad();

    selected = 0;

    std::string title = "Adventures";
    for (UINT x = 0; x < title.size(); x++)
    {
        DisplayChar* c = getTextDisplay()->getDisplayChar(10 + x * 3, 3);
        c->setChar(title[x]);
        c->setScale(3);
        c->setColor(Color(0.5f, 0.9f, 0.8f));
    }

    getTextDisplay()->write(2, 6, "   ____________________________________________________");
    getTextDisplay()->write(2, 7, "  /                                                   /");
    getTextDisplay()->write(2, 8, " /                                                   /");
    getTextDisplay()->write(2, 9, "/___________________________________________________/");

    /*
    getTextDisplay()->write(2, 11, "                  /\\");
    getTextDisplay()->write(2, 12, "  <C> the quest for the bow");
    getTextDisplay()->write(2, 14, "[ <S> the tree of truth             ]");
    getTextDisplay()->write(2, 16, "  <C> super amazing wagon adventure");
    getTextDisplay()->write(2, 18, "  <C> this is a long filename an...  ");
    getTextDisplay()->write(2, 20, "  <S> der geschichte");
    getTextDisplay()->write(2, 22, "  <S> yet another entry");
    getTextDisplay()->write(2, 23, "                  \\/");
    */

    for (int i = 0; i < 13; i++)
    {
        getTextDisplay()->write(40, 11 + i, i % 2 ? "/" : "\\");
    }

    getTextDisplay()->write(42, 13, "  Play");
    getTextDisplay()->write(42, 15, "  Edit");
    getTextDisplay()->write(42, 17, "[ Compile     ]");
    getTextDisplay()->write(42, 19, "  Open Folder");
    getTextDisplay()->write(42, 21, "  Delete");

    getTextDisplay()->write(4, 26, "The Tree of Truth");
    getTextDisplay()->write(2, 28, "There is only one tree, knowing it all. The legendary");
    getTextDisplay()->write(2, 29, "tree of the truth. He knows not only what you ate for");
    getTextDisplay()->write(2, 30, "lunch, but also what the best way to ride a giant horse");
    getTextDisplay()->write(2, 31, "with wings is! Amazing, right?");

    searchBar = new LineInput(getTextDisplay(), 8, 5, getTextDisplay()->getWidth() - 11);
    advSelection = new ListSelection(getTextDisplay(), 2, 11, 37, 6);

    loadAdventures();
}

void AdventureSelection::notifyUnload()
{
    GameDisplayer::notifyUnload();

    unloadAdventures();

    delete searchBar;
    searchBar = NULL;
    delete advSelection;
    advSelection = NULL;
}

void AdventureSelection::update(float deltaTime)
{
    searchBar->update();
    advSelection->update();
}

void AdventureSelection::pressChar(byte c)
{
    searchBar->pressChar(c);
}

void AdventureSelection::pressKey(byte key)
{
    searchBar->pressKey(key);
    advSelection->pressKey(key);

    if (searchBar->changed())
        loadAdventures();

    switch (key)
    {
    case VK_F5:
        loadAdventures();
        break; 
    }
}
