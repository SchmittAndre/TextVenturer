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

    searchBar = new LineInput(getTextDisplay(), ivec2(5, 8), getTextDisplay()->getWidth() - 11);
    searchBar->enable();
    searchBar->addOnChange(this, onSearchBarChanged);

    adventureSelection = new ListSelection(getTextDisplay(), ivec2(1, 11), getTextDisplay()->getWidth() - 2, 6); 
    adventureSelection->enable();
    adventureSelection->addOnChange(this, onAdventureSelectionChange);
    adventureSelection->addOnSelect(this, onAdventureSelect);

    actionSelection = new ListSelection(getTextDisplay(), ivec2(getTextDisplay()->getWidth(), 11), 16, 6);
    actionSelection->addOnSelect(this, onActionSelect);
    actions.push_back(new ActionPlay(this));
    actions.push_back(new ActionErrorLog(this));
    actions.push_back(new ActionCompile(this));
    actions.push_back(new ActionRename(this));
    actions.push_back(new ActionTextEditor(this));
    actions.push_back(new ActionWinExplorer(this));
    actions.push_back(new ActionDelete(this));

    infoBox = new LimitedTextBox(getTextDisplay(), ivec2(2, 26), getTextDisplay()->getWidth() - 4, 5);

    loadAdventures();

    actionsVisible = false;
    actionsTimer = 0;
}

void AdventureSelection::notifyUnload()
{                          
    GameDisplayer::notifyUnload();

    for (ActionBase* action : actions)
        delete action;

    delete searchBar;
    delete adventureSelection;
    delete actionSelection;
    delete infoBox;
}

void AdventureSelection::update(float deltaTime)
{
    searchBar->update(deltaTime);

    if (regenList)
        generateList();

    actionsTimer -= deltaTime;
    while (actionsTimer < 0)
    {
        actionsTimer += 0.015f;
        if (actionsVisible)
        {
            if (actionSelection->getPos().x > static_cast<int>(getTextDisplay()->getWidth() - actionSelection->getWidth()) - 1)
            {
                actionSelection->setVisible(true);
                actionSelection->setPos(ivec2(actionSelection->getPos().x - 1, actionSelection->getPos().y));
                adventureSelection->setWidth(adventureSelection->getWidth() - 1);
            }
        }
        else
        {
            if (actionSelection->getPos().x < static_cast<int>(getTextDisplay()->getWidth()))
            {
                actionSelection->setPos(ivec2(actionSelection->getPos().x + 1, actionSelection->getPos().y));
                adventureSelection->setWidth(adventureSelection->getWidth() + 1);
            }
            else
            {
                actionSelection->setVisible(false);
            }
        }
    }

    actionSelection->update(deltaTime);
    adventureSelection->update(deltaTime);

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
            actionsVisible = false;
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
    auto a = static_cast<AdventureSelection::NamedAdventure*>(t->adventureSelection->getSelectedData());
    
    if (a->getState() == AdventureSelection::NamedAdventure::stLoading)
    {
        t->adventureSelection->unlockSelection();
        return;
    }
    t->actionSelection->delAll();

    for (AdventureSelection::ActionBase* action : t->actions)
        if (action->canExecute(a))
            t->actionSelection->add(action->getDisplayString(), action);

    t->searchBar->disable();
    t->actionSelection->enable();
    t->actionsVisible = true;
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
    auto action = static_cast<AdventureSelection::ActionBase*>(t->actionSelection->getSelectedData());
    action->execute(static_cast<AdventureSelection::NamedAdventure*>(t->adventureSelection->getSelectedData()));                                                                  
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

AdventureSelection::ActionBase::ActionBase(AdventureSelection * adventureSelection)
{
    this->adventureSelection = adventureSelection;
}

AdventureSelection::ActionBase::~ActionBase()
{
}

AdventureSelection * AdventureSelection::ActionBase::getAdventureSelection()
{
    return adventureSelection;
}

void AdventureSelection::ActionBase::execute(NamedAdventure * adventure)
{
    ErrorDialog("[" + getDisplayString() + "] not implemented!");
    getAdventureSelection()->actionSelection->unlockSelection();
}

bool AdventureSelection::ActionPlay::canExecute(NamedAdventure * adventure)
{
    return adventure->getState() == NamedAdventure::stLoadSuccess;
}

void AdventureSelection::ActionPlay::execute(NamedAdventure * adventure)
{
    getAdventureSelection()->getControler()->getCmdLine()->setAdventure(adventure->getAdventureOwnership());
    getAdventureSelection()->unloadAdventures();
    getAdventureSelection()->getControler()->changeDisplayer(Controler::dtAdventure);
}

std::string AdventureSelection::ActionPlay::getDisplayString()
{
    return "Play";
}

bool AdventureSelection::ActionErrorLog::canExecute(NamedAdventure * adventure)
{
    return adventure->getState() == NamedAdventure::stLoadFailure;
}

std::string AdventureSelection::ActionErrorLog::getDisplayString()
{
    return "Error-Log";
}

bool AdventureSelection::ActionCompile::canExecute(NamedAdventure * adventure)
{
    return adventure->getState() == NamedAdventure::stLoadSuccess
        && adventure->getFileType() == NamedAdventure::ftScript;
}

std::string AdventureSelection::ActionCompile::getDisplayString()
{
    return "Compile";
}

bool AdventureSelection::ActionRename::canExecute(NamedAdventure * adventure)
{
    return true;
}

std::string AdventureSelection::ActionRename::getDisplayString()
{
    return "Rename";
}

bool AdventureSelection::ActionTextEditor::canExecute(NamedAdventure * adventure)
{
    return adventure->getFileType() == NamedAdventure::ftScript;
}

std::string AdventureSelection::ActionTextEditor::getDisplayString()
{
    return "Text-Editor";
}

bool AdventureSelection::ActionWinExplorer::canExecute(NamedAdventure * adventure)
{
    return true;
}

std::string AdventureSelection::ActionWinExplorer::getDisplayString()
{
    return "Win-Explorer";
}

bool AdventureSelection::ActionDelete::canExecute(NamedAdventure * adventure)
{
    return true;
}

std::string AdventureSelection::ActionDelete::getDisplayString()
{
    return "Delete";
}
