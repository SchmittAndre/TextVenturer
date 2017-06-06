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
#include "Game.h"
#include "Window.h"
#include "AdventureErrorLog.h"

#include "AdventureSelection.h"

// NamedAdventure

std::string AdventureSelection::NamedAdventure::getDisplayName() const
{          
    std::string afilename = strconv(getName());

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

bool AdventureSelection::NamedAdventure::compare(const NamedAdventure & a, const NamedAdventure & b)
{
    return a.filename < b.filename;
}

AdventureSelection::NamedAdventure::FileType AdventureSelection::NamedAdventure::getFileType() const
{
    std::wstring ext = extractFileExtension(getName());

    if (ext == L"txvs")
        return ftScript;
    if (ext == L"txvc")
        return ftCompiled;

    return ftUnknown; // not possible    
}

Adventure & AdventureSelection::NamedAdventure::getAdventure() const
{
    if (!adventure)
        throw(EAdventureNotInitialized);
    return *adventure;
}

Adventure & AdventureSelection::NamedAdventure::getAdventureOwnership() 
{
    Adventure & a = getAdventure();
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

    delete adventure;
    adventure = NULL;

    std::thread([&]
    {
        loadingSection.lock();
        setState(stLoading);
        
        try
        {    
            switch (getFileType())
            {
            case ftScript:
                adventure = new Adventure(L"data\\adventure\\" + filename);
                break;
            case ftCompiled:
                adventure = new Adventure(L"data\\compiled\\" + filename);
                break;
            }

            if (adventure->isInitialized())
                setState(stLoadSuccess);
            else
                setState(stLoadFailure);
        }
        catch (...)
        {
            adventureSelection.getControler().getGame().getWindow().showException();
            setState(stLoadFatal);
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

AdventureSelection::NamedAdventure::NamedAdventure(std::wstring filename, AdventureSelection & adventureSelection)
    : filename(filename)
    , adventureSelection(adventureSelection)
    , adventure(NULL)
    , state(stNotLoaded)
{
}

AdventureSelection::NamedAdventure::~NamedAdventure()
{
    loadingSection.lock();          
    delete adventure;
    loadingSection.unlock();
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
            adventures.push_back(new NamedAdventure(file.cFileName, *this));
            adventures.back()->addOnStateChanged(this, onAdventureStateChanged);               
   
        }

    std::sort(adventures.begin(), adventures.end(), [](NamedAdventure * a, NamedAdventure * b) {
        return a->getName() < b->getName();
    });
    
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
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(0.8,0.2,0.8)$shaking_on()  Loading...");
    infoBoxSection.unlock();
}

void AdventureSelection::infoBoxError()
{
    infoBoxSection.lock();
    Adventure & adventure = static_cast<NamedAdventure*>(adventureSelection->getSelectedData())->getAdventure();
    infoBox->clear();
    std::string errorCount = std::to_string(adventure.getErrorLog().size()) + " Error";
    if (adventure.getErrorLog().size() > 1)
        errorCount += "s";
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(1.0,0.3,0.3)  " + errorCount + "$reset()");
    infoBox->writeToBuffer("");
    infoBox->writeToBuffer("$scale(1)$rgb(0.9,0.3,0.3) Check the Error-Log for further information!");
    infoBoxSection.unlock();
}

void AdventureSelection::infoBoxFatal()
{
    infoBoxSection.lock();
    infoBox->clear();
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(1.0,0.3,0.3)  UNKNOWN ERROR!");
    infoBoxSection.unlock();
}

void AdventureSelection::infoBoxDescription()
{
    infoBoxSection.lock();
    Adventure & adventure = static_cast<NamedAdventure*>(adventureSelection->getSelectedData())->getAdventure();
    infoBox->clear();
    infoBox->writeToBuffer(adventure.getTitle() + "$reset()");
    infoBox->writeToBuffer("");
    infoBox->writeToBuffer(adventure.getDescription());
    infoBoxSection.unlock();
}

void AdventureSelection::infoBoxNoAdventure()
{
    infoBoxSection.lock();
    infoBox->clear();
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(0.7,0.7,0.7)  No adventure!");
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
    case AdventureSelection::NamedAdventure::stLoadFatal:
        infoBoxFatal();
        break;
    }
}

AdventureSelection::AdventureSelection(Controler & controler)
    : GameDisplayer(controler)
    , keepLoaded(false)
{
}

AdventureSelection::~AdventureSelection()
{
    if (getControler().getCurrentDisplayerType() == Controler::dtErrorLog)
    {
        keepLoaded = false;
        notifyUnload();
    }
}

void AdventureSelection::notifyLoad()
{
    GameDisplayer::notifyLoad();

    TextDisplay::State state;
    state.color = Color(0.5f, 0.9f, 0.8f);
    state.scale = vec2(3, 3);
    state.offsetMovement = vec2(3, 0);
    state.rainbow = 0.05f;
    getTextDisplay().writeAll(10, 3, "Adventures", state);

    getTextDisplay().write(2, 6, "   ____________________________________________________");
    getTextDisplay().write(2, 7, "  /                                                   /");
    getTextDisplay().write(2, 8, " /                                                   /");
    getTextDisplay().write(2, 9, "/___________________________________________________/");

    if (keepLoaded)
    {
        keepLoaded = false;
        searchBar->notifyChanges();
        adventureSelection->notifyChanges();
        actionSelection->notifyChanges(); 
        updateSelectedAdventure();
        actionSelection->unlockSelection();
        return;
    }

    searchBar = new LineInput(getTextDisplay(), ivec2(5, 8), getTextDisplay().getWidth() - 11);
    searchBar->enable();
    searchBar->addOnChange(this, onSearchBarChanged);

    adventureSelection = new ListSelection(getTextDisplay(), ivec2(1, 11), getTextDisplay().getWidth() - 2, 6); 
    adventureSelection->enable();
    adventureSelection->addOnChange(this, onAdventureSelectionChange);
    adventureSelection->addOnSelect(this, onAdventureSelect);

    actionSelection = new ListSelection(getTextDisplay(), ivec2(getTextDisplay().getWidth(), 11), 16, 6);
    actionSelection->addOnSelect(this, onActionSelect);
    actions.push_back(new ActionPlay(*this));
    actions.push_back(new ActionErrorLog(*this));
    actions.push_back(new ActionCompile(*this));
    actions.push_back(new ActionRename(*this));
    actions.push_back(new ActionTextEditor(*this));
    actions.push_back(new ActionWinExplorer(*this));
    actions.push_back(new ActionDelete(*this));

    infoBox = new LimitedTextBox(getTextDisplay(), ivec2(2, 26), getTextDisplay().getWidth() - 4, 5);
    infoBox->setInstant(true);
    
    loadAdventures();

    actionsVisible = false;
    actionsTimer = 0;
}

void AdventureSelection::notifyUnload()
{                          
    GameDisplayer::notifyUnload();

    if (keepLoaded)
        return;

    unloadAdventures();

    for (ActionBase * action : actions)
        delete action;
    actions.clear();

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
            if (actionSelection->getPos().x > static_cast<int>(getTextDisplay().getWidth() - actionSelection->getWidth()) - 1)
            {
                actionSelection->setVisible(true);
                actionSelection->setPos(ivec2(actionSelection->getPos().x - 1, actionSelection->getPos().y));
                adventureSelection->setWidth(adventureSelection->getWidth() - 1);
            }
        }
        else
        {
            if (actionSelection->getPos().x < static_cast<int>(getTextDisplay().getWidth()))
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
        if (!adventureSelection->selectionIsLocked())
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
            getControler().changeDisplayer(Controler::dtMainMenu);
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

    for (AdventureSelection::ActionBase * action : t->actions)
        if (action->canExecute(*a))
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
    action->execute(*static_cast<AdventureSelection::NamedAdventure*>(t->adventureSelection->getSelectedData()));                                                                  
}

void onAdventureStateChanged(void * self, void * sender)
{
    auto t = static_cast<AdventureSelection*>(self);
    auto a = static_cast<AdventureSelection::NamedAdventure*>(sender);
    
    if (!t->active())
        return;
    
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
            case AdventureSelection::NamedAdventure::stLoadFatal:
                t->infoBoxFatal();
                break;
            }
        }
    }
}

AdventureSelection::ActionBase::ActionBase(AdventureSelection & adventureSelection)
    : adventureSelection(adventureSelection)
{
}

AdventureSelection::ActionBase::~ActionBase()
{
}

AdventureSelection & AdventureSelection::ActionBase::getAdventureSelection() const
{
    return adventureSelection;
}

void AdventureSelection::ActionBase::execute(NamedAdventure & adventure) const
{
    getAdventureSelection().actionSelection->unlockSelection();
    throw(ENotImplemented, "Action " + getDisplayString());
}

bool AdventureSelection::ActionPlay::canExecute(NamedAdventure & adventure) const
{
    return adventure.getState() == NamedAdventure::stLoadSuccess;
}

void AdventureSelection::ActionPlay::execute(NamedAdventure & adventure) const
{
    getAdventureSelection().getControler().getCmdLine().setAdventure(adventure.getAdventureOwnership());
    getAdventureSelection().unloadAdventures();
    getAdventureSelection().getControler().changeDisplayer(Controler::dtAdventure);
}

std::string AdventureSelection::ActionPlay::getDisplayString() const
{
    return "Play";
}

bool AdventureSelection::ActionErrorLog::canExecute(NamedAdventure & adventure) const
{
    return adventure.getState() == NamedAdventure::stLoadFailure;
}

void AdventureSelection::ActionErrorLog::execute(NamedAdventure & adventure) const
{
    getAdventureSelection().getControler().getErrorLog().setAdventure(adventure.getAdventure());
    getAdventureSelection().keepLoaded = true;
    getAdventureSelection().getControler().changeDisplayer(Controler::dtErrorLog);
}

std::string AdventureSelection::ActionErrorLog::getDisplayString() const
{
    return "Error-Log";
}

bool AdventureSelection::ActionCompile::canExecute(NamedAdventure & adventure) const
{
    return adventure.getState() == NamedAdventure::stLoadSuccess
        && adventure.getFileType() == NamedAdventure::ftScript;
}

std::string AdventureSelection::ActionCompile::getDisplayString() const
{
    return "Compile";
}

bool AdventureSelection::ActionRename::canExecute(NamedAdventure & adventure) const
{
    return true;
}

std::string AdventureSelection::ActionRename::getDisplayString() const
{
    return "Rename";
}

bool AdventureSelection::ActionTextEditor::canExecute(NamedAdventure & adventure) const
{
    return adventure.getFileType() == NamedAdventure::ftScript;
}

std::string AdventureSelection::ActionTextEditor::getDisplayString() const
{
    return "Text-Editor";
}

bool AdventureSelection::ActionWinExplorer::canExecute(NamedAdventure & adventure) const
{
    return true;
}

std::string AdventureSelection::ActionWinExplorer::getDisplayString() const
{
    return "Win-Explorer";
}

bool AdventureSelection::ActionDelete::canExecute(NamedAdventure & adventure) const
{
    return true;
}

std::string AdventureSelection::ActionDelete::getDisplayString() const
{
    return "Delete";
}
