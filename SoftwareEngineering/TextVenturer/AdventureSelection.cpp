#include "stdafx.h"

#include "Adventure.h"
#include "FileFinder.h"
#include "TextDisplay.h"         
#include "DisplayChar.h"
#include "LineInput.h"
#include "ListSelection.h"
#include "TextBox.h"
#include "Controler.h"
#include "CmdLine.h"

#include "AdventureSelection.h"

const std::string AdventureSelection::actionStrings[ACTION_COUNT] = {
    "Play",
    "Compile",
    "Rename",
    "Text-Editor",
    "Win-Explorer",
    "Delete"
};

// NamedAdventure

std::string AdventureSelection::NamedAdventure::getDisplayName() const
{          
    std::string afilename = getNameAnsi();

    size_t end = afilename.rfind('.');
    if (end == std::string::npos)
        return "???"; // not possible

    std::string name(afilename.c_str(), end);
    
    switch (getFileType())
    {
    case ftScript:
        return "<S> " + name;
    case ftCompiled:
        return "<C> " + name;  
    }
    return "<?> " + name;
}

AdventureSelection::NamedAdventure::State AdventureSelection::NamedAdventure::getState() const
{
    return state;
}

bool AdventureSelection::NamedAdventure::compare(const NamedAdventure* a, const NamedAdventure* b)
{
    return a->filename < b->filename;
}

AdventureSelection::NamedAdventure::FileType AdventureSelection::NamedAdventure::getFileType() const
{
    std::string afilename = getNameAnsi();

    size_t end = afilename.rfind('.');
    if (end == std::string::npos)
        return ftUnknown; // not possible

    std::string ext = std::string(afilename.begin() + end + 1, afilename.end());

    if (ext == "txvs")
        return ftScript;
    if (ext == "txvc")
        return ftCompiled;

    return ftUnknown; // not possible    
}

Adventure * AdventureSelection::NamedAdventure::getAdventure() const
{
    return adventure;
}

Adventure * AdventureSelection::NamedAdventure::getAdventureOwnership() 
{
    Adventure* a = adventure;
    adventure = NULL;
    state = stNotLoaded;
    return a;
}

void AdventureSelection::NamedAdventure::unloadAdventure()
{
    delete adventure;
    adventure = NULL;
    state = stNotLoaded;
}

void AdventureSelection::NamedAdventure::loadAdventure()
{
    if (state != stNotLoaded)
        return;

    std::thread([&]
    {
        loadingSection.lock();
        setState(stLoading);
        
        adventure = new Adventure();
        
        bool success = false;
        switch (getFileType())
        {
        case ftScript:
            success = adventure->loadFromFile(L"data\\adventure\\" + filename);
            break;
        case ftCompiled:
            success = adventure->loadState(L"data\\compiled\\" + filename);
            break;
        }

        if (success)
            setState(stLoadSuccess);
        else
        {
            delete adventure;
            adventure = NULL;
            setState(stLoadFailure);
        }
        loadingSection.unlock();
    }).detach();
}

void AdventureSelection::NamedAdventure::addOnStateChanged(void * self, EventFuncNotify func)
{
    onStateChanged.insert({self, func});
}

void AdventureSelection::NamedAdventure::delOnStateChanged(void * self, EventFuncNotify func)
{
    onStateChanged.erase({ self, func });
}

void AdventureSelection::NamedAdventure::setState(State state)
{
    this->state = state;
    for (auto e : onStateChanged)
        e.func(e.self, this);
}

AdventureSelection::NamedAdventure::NamedAdventure(std::wstring filename)
{
    this->filename = filename;
    adventure = NULL;
    state = stNotLoaded;
}

AdventureSelection::NamedAdventure::~NamedAdventure()
{
    loadingSection.lock();          
    delete adventure;
    loadingSection.unlock();
}

std::string AdventureSelection::NamedAdventure::getNameAnsi() const
{
    std::string afilename;
    afilename.reserve(filename.length());
    for (auto c : filename)
        afilename += static_cast<WORD>(c) < 0xFF ? static_cast<char>(c) : '?';
    return afilename;
}

std::wstring AdventureSelection::NamedAdventure::getName() const
{
    return filename;
}

// AdventureSelection

void AdventureSelection::loadAdventures()
{
    unloadAdventures();

    std::vector<std::wstring> paths = {
        L"data\\compiled\\*.txvc",
        L"data\\adventure\\*.txvs"
    };     
   
    for (std::wstring path : paths)
        for (auto file : FileFinder(path))
        {               
            adventures.push_back(new NamedAdventure(file.cFileName));
            adventures.back()->addOnStateChanged(this, onAdventureStateChanged);               
   
        }

    std::sort(adventures.begin(), adventures.end(), NamedAdventure::compare);     
    
    adventureSelection->delAll();

    regenList = true;
}

void AdventureSelection::unloadAdventures()
{
    for (auto entry : adventures)
        delete entry;
    adventures.clear();
}

void AdventureSelection::generateList()
{
    NamedAdventure* oldSelection = NULL;
    if (adventureSelection->isSelected())
        oldSelection = static_cast<NamedAdventure*>(adventureSelection->getSelectedData());
    adventureSelection->delAll();

    std::wstring input;
    for (auto c : searchBar->getInput())
        input += toupper(c);

    for (auto & entry : adventures)
    {
        std::wstring nameonly(entry->getName().c_str(), entry->getName().rfind('.'));
        transform(nameonly.begin(), nameonly.end(), nameonly.begin(), toupper);
        if (input == L"" || nameonly.find(input) != std::string::npos)
            adventureSelection->add(entry->getDisplayName(), entry);
    }

    if (oldSelection)
        adventureSelection->setSelectedByData(oldSelection);
    
    if (adventureSelection->isSelected())
        updateSelectedAdventure();
    else
        infoBoxNoAdventure();

    regenList = false;
}

void AdventureSelection::infoBoxLoading()
{
    infoBoxSection.lock();
    infoBox->clear();
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(0.8,0.2,0.8)$shaking_on()$delay(0)  Loading...");
    infoBoxSection.unlock();
}

void AdventureSelection::infoBoxError()
{
    infoBoxSection.lock();
    infoBox->clear();
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(1.0,0.3,0.3)$delay(0)  ERROR!");
    infoBoxSection.unlock();
}

void AdventureSelection::infoBoxDescription()
{
    infoBoxSection.lock();
    Adventure* adventure = static_cast<NamedAdventure*>(adventureSelection->getSelectedData())->getAdventure();
    infoBox->clear();
    infoBox->writeToBuffer("$delay(0)" + adventure->getTitle() + "$reset()");
    infoBox->writeToBuffer("");
    infoBox->writeToBuffer("$delay(0)" + adventure->getDescription());
    infoBoxSection.unlock();
}

void AdventureSelection::infoBoxNoAdventure()
{
    infoBoxSection.lock();
    infoBox->clear();
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(0.7,0.7,0.7)$delay(0)  No adventure!");
    infoBoxSection.unlock();    
}

void AdventureSelection::updateSelectedAdventure()
{
    auto adventure = static_cast<AdventureSelection::NamedAdventure*>(adventureSelection->getSelectedData());
    switch (adventure->getState())
    {
    case AdventureSelection::NamedAdventure::stNotLoaded:
        adventure->loadAdventure();
        break;
    case AdventureSelection::NamedAdventure::stLoading:
        infoBoxLoading();
        break;
    case AdventureSelection::NamedAdventure::stLoadSuccess:
        infoBoxDescription();
        break;
    case AdventureSelection::NamedAdventure::stLoadFailure:
        infoBoxError();
        break;
    }
}

AdventureSelection::AdventureSelection(Controler* controler)
    : GameDisplayer(controler)
{
}

AdventureSelection::~AdventureSelection()
{                              
    unloadAdventures();
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

    /*
    getTextDisplay()->write(42, 13, "  Play");
    getTextDisplay()->write(42, 15, "  Edit");
    getTextDisplay()->write(42, 17, "[ Compile     ]");
    getTextDisplay()->write(42, 19, "  Open Folder");
    getTextDisplay()->write(42, 21, "  Delete");
    */
    /*
    getTextDisplay()->write(4, 26, "The Tree of Truth");
    getTextDisplay()->write(2, 28, "There is only one tree, knowing it all. The legendary");
    getTextDisplay()->write(2, 29, "tree of the truth. He knows not only what you ate for");
    getTextDisplay()->write(2, 30, "lunch, but also what the best way to ride a giant horse");
    getTextDisplay()->write(2, 31, "with wings is! Amazing, right?");
    */

    searchBar = new LineInput(getTextDisplay(), 8, 5, getTextDisplay()->getWidth() - 11);
    searchBar->enable();
    searchBar->addOnChange(this, onSearchBarChanged);

    adventureSelection = new ListSelection(getTextDisplay(), 2, 11, 37, 6);
    adventureSelection->enable();
    adventureSelection->addOnChange(this, onAdventureSelectionChange);
    adventureSelection->addOnSelect(this, onAdventureSelect);

    actionSelection = new ListSelection(getTextDisplay(), 42, 11, 16, 6);
    for (std::string action : actionStrings)
        actionSelection->add(action);
    actionSelection->addOnSelect(this, onActionSelect);

    infoBox = new LimitedTextBox(getTextDisplay(), 2, 26, getTextDisplay()->getWidth() - 4, 5);

    loadAdventures();
}

void AdventureSelection::notifyUnload()
{                          
    GameDisplayer::notifyUnload();

    delete searchBar;
    delete adventureSelection;
    delete actionSelection;
    delete infoBox;
}

void AdventureSelection::update(float deltaTime)
{
    searchBar->update();
    actionSelection->update();

    if (regenList)
        generateList();

    adventureSelection->update();

    infoBoxSection.lock();
    infoBox->update(deltaTime);
    infoBoxSection.unlock();
}

void AdventureSelection::pressChar(byte c)
{
    searchBar->pressChar(c);
}

void AdventureSelection::pressKey(byte key)
{
    searchBar->pressKey(key);
    actionSelection->pressKey(key);
    adventureSelection->pressKey(key);

    switch (key)
    {
    case VK_F5:
        if (adventureSelection->isEnabled())
            loadAdventures();
        break; 
    case VK_ESCAPE:
        if (adventureSelection->selectionIsLocked())
        {
            adventureSelection->unlockSelection();
            searchBar->enable();
            actionSelection->disable(); 
        }
        else
        {
            unloadAdventures();
            getControler()->changeDisplayer(Controler::dtMainMenu);
        }
        break;
    }
}

void onSearchBarChanged(void * self, void * sender)
{
    auto t = static_cast<AdventureSelection*>(self);
    t->regenList = true;
}

void onAdventureSelect(void * self, void * sender)
{
    auto t = static_cast<AdventureSelection*>(self);
    t->searchBar->disable();
    t->actionSelection->enable();
}

void onAdventureSelectionChange(void * self, void * sender)
{
    auto t = static_cast<AdventureSelection*>(self);
    if (t->adventureSelection->isSelected())
        t->updateSelectedAdventure();
}

void onActionSelect(void * self, void * sender)
{
    auto t = static_cast<AdventureSelection*>(self);
    switch (static_cast<AdventureSelection::Action>(t->actionSelection->getIndex()))
    {
    case AdventureSelection::acPlay:
    {
        Adventure* adventure = static_cast<AdventureSelection::NamedAdventure*>(t->adventureSelection->getSelectedData())->getAdventureOwnership();
        t->unloadAdventures();
        t->getControler()->getCmdLine()->setAdventure(adventure);
        t->getControler()->changeDisplayer(Controler::dtAdventure);
        break;                                                       
    }
    case AdventureSelection::acCompile:
        t->adventureSelection->unlockSelection();
        break;
    case AdventureSelection::acRename:
        t->adventureSelection->unlockSelection();
        break;
    case AdventureSelection::acTextEditor:
        t->adventureSelection->unlockSelection();
        break;
    case AdventureSelection::acWinExplorer:
        t->adventureSelection->unlockSelection();
        break;
    case AdventureSelection::acDelete:
        t->adventureSelection->unlockSelection();
        break;
    }
}

void onAdventureStateChanged(void * self, void * sender)
{
    auto t = static_cast<AdventureSelection*>(self);
    auto a = static_cast<AdventureSelection::NamedAdventure*>(sender);
    if (t->adventureSelection->isSelected())
    {          
        auto adventure = static_cast<AdventureSelection::NamedAdventure*>(t->adventureSelection->getSelectedData());
        
        if (adventure == a)
        {
            switch (adventure->getState())
            {
            case AdventureSelection::NamedAdventure::stLoading:
                t->infoBoxLoading();
                break;
            case AdventureSelection::NamedAdventure::stLoadSuccess:
                t->infoBoxDescription();
                break;
            case AdventureSelection::NamedAdventure::stLoadFailure:
                t->infoBoxError();
                break;
            }
        }
    }
}
