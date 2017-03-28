#include "stdafx.h"

#include "Adventure.h"
#include "FileFinder.h"
#include "TextDisplay.h"         
#include "DisplayChar.h"

#include "AdventureSelection.h"

void AdventureSelection::unloadAdventures()
{
    for (Adventure* adventure : adventures)
        delete adventure;
    adventures.clear();             
}

void AdventureSelection::loadAdventures()
{
    unloadAdventures();
    
    for (auto file : FileFinder(L"data\\adventure\\*.txvs"))
    {
        Adventure* adventure = new Adventure();
        // TODO: Exceptions! Have a try-catch around the loadFromFile 
        //       and then display the error message where the description would be
        if (adventure->loadFromFile(std::wstring(L"data\\adventure\\") + file.cFileName))
            adventures.push_back(adventure);
        else
            ErrorDialog(std::wstring(L"Could not load ") + file.cFileName);
    }     

    listChanged = true;
}

AdventureSelection::AdventureSelection(Controler* controler)
    : GameDisplayer(controler)
{
}

AdventureSelection::~AdventureSelection()
{
    unloadAdventures();
}

void AdventureSelection::notifySwitch()
{
    loadAdventures();
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
    getTextDisplay()->write(2, 8, " /  > _                                              /");
    getTextDisplay()->write(2, 9, "/___________________________________________________/");

    getTextDisplay()->write(2, 11, "                  /\\");
    getTextDisplay()->write(2, 12, "  <C> the quest for the bow");
    getTextDisplay()->write(2, 14, "[ <S> the tree of truth             ]");
    getTextDisplay()->write(2, 16, "  <C> super amazing wagon adventure");
    getTextDisplay()->write(2, 18, "  <C> this is a long filename an...  ");
    getTextDisplay()->write(2, 20, "  <S> der geschichte");
    getTextDisplay()->write(2, 22, "  <S> yet another entry");
    getTextDisplay()->write(2, 23, "                  \\/");

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

}

void AdventureSelection::update(float deltaTime)
{
    if (listChanged)
    {
        
    }
}
