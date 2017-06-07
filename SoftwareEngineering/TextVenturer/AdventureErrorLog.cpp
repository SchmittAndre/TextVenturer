#include "stdafx.h"

#include "Adventure.h"
#include "Controler.h"
#include "ListSelection.h"
#include "TextDisplay.h"
#include "TextBox.h"
#include "Game.h"
#include "Window.h"

#include "AdventureErrorLog.h"

void AdventureErrorLog::showReloading()
{
    infoBox->clear();                          
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(0.8,0.2,0.8)$shaking_on()  Reloading...");
}

void AdventureErrorLog::showNoError()
{
    infoBox->clear();
    infoBox->writeToBuffer("$scale(2)$offset_movement(1,0)$rgb(0.4,0.9,0.4)Adventure loaded successfully!");
}

void AdventureErrorLog::showDescription()
{
    const Adventure::ErrorLogEntry & error = adventure->getErrorLog().at(reinterpret_cast<size_t>(errorList->getSelectedData()));
    infoBox->clear();
    infoBox->writeToBuffer("$rgb(0.2,1.0,0.2)Line: $rgb(0.4,0.9,0.4)[" + std::to_string(error.lineinfo.line) +
                                                                  ", " + std::to_string(error.lineinfo.col) + "]");
    infoBox->writeToBuffer("");
    infoBox->writeToBuffer("$rgb(0.2,0.2,1.0)Path: $rgb(0.4,0.4,0.9)" + error.location.getFullPath());
    infoBox->writeToBuffer("");
    infoBox->writeToBuffer("$rgb(1.0,0.2,0.2)  [ " + error.getTypeName() + " Error ]$reset()");
    infoBox->writeToBuffer("$rgb(0.9,0.4,0.4)" + error.msg);
}

void AdventureErrorLog::reload()
{
    errorList->delAll();
    showReloading();

    std::thread([&]() 
    {
        reloadLock.lock();          

        try
        {
            adventure->reload(); 
            listUpdateLock.lock();
            updateList();
            listUpdateLock.unlock();
            if (adventure->isInitialized())
                showNoError();
            else
                showDescription();
        }
        catch (...)
        {
            getControler().getGame().getWindow().showException();
        }              

        reloadLock.unlock();
    }).detach();
}

void AdventureErrorLog::updateList()
{
    errorList->delAll();
    size_t i = 0;
    for (const Adventure::ErrorLogEntry & error : adventure->getErrorLog())
        errorList->add(error.msg, reinterpret_cast<void*>(i++));
}

AdventureErrorLog::AdventureErrorLog(Controler & controler)
    : GameDisplayer(controler)
{
}

void AdventureErrorLog::setAdventure(Adventure & adventure)
{
    this->adventure = &adventure;
}

void AdventureErrorLog::notifyLoad()
{
    assert(adventure);

    errorList = new ListSelection(getTextDisplay(), 
        ivec2(2, 1), 
        getTextDisplay().getWidth() - 4, 
        9);
    errorList->enable();
    errorList->addOnSelect(this, onErrorListSelect);
    errorList->addOnChange(this, onErrorListChange);

    infoBox = new LimitedTextBox(getTextDisplay(), 
        ivec2(2, getTextDisplay().getHeight() - 12), 
        getTextDisplay().getWidth() - 4, 
        10);
    infoBox->setInstant(true);

    updateList();
    showDescription();
}

void AdventureErrorLog::notifyUnload()
{
    adventure = NULL;

    delete errorList;
    errorList = NULL;

    delete infoBox;
    infoBox = NULL;
}

void AdventureErrorLog::pressKey(byte key)
{
    listUpdateLock.lock();
    errorList->pressKey(key);
    listUpdateLock.unlock();

    switch (key)
    {
    case VK_F5:
        if (reloadLock.try_lock())
        {
            reloadLock.unlock();
            reload();
        }
        break;
    case VK_ESCAPE:
        if (reloadLock.try_lock())
        {
            reloadLock.unlock();
            getControler().changeDisplayer(Controler::dtAdventureSelection);
        }
        break;
    }
}

void AdventureErrorLog::update(float deltaTime)
{
    errorList->update(deltaTime);
    infoBox->update(deltaTime);
}

void onErrorListSelect(void * self, void * sender)
{
    auto t = static_cast<AdventureErrorLog*>(self);
    t->errorList->unlockSelection();
}

void onErrorListChange(void * self, void * sender)
{
    auto t = static_cast<AdventureErrorLog*>(self);
    auto s = static_cast<ListSelection*>(sender);
    if (s->isSelected())
        t->showDescription();
}
