#include "stdafx.h"

#include "Adventure.h"
#include "Controler.h"
#include "ListSelection.h"
#include "TextDisplay.h"
#include "TextBox.h"

#include "AdventureErrorLog.h"

void AdventureErrorLog::showDescription()
{
    const Adventure::ErrorLogEntry & error = adventure->getErrorLog().at(reinterpret_cast<size_t>(errorList->getSelectedData()));
    infoBox->clear();
    infoBox->writeToBuffer("  $rgb(1.0,0.2,0.2)" + error.getTypeName() + " Error:");
    infoBox->writeToBuffer("$rgb(0.9,0.4,0.4)" + error.msg);
    infoBox->writeToBuffer("");
    infoBox->writeToBuffer("  $rgb(0.2,0.2,1.0)Path:");
    infoBox->writeToBuffer("$rgb(0.4,0.4,0.9)" + error.location.getFullPath());
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
        10);
    errorList->enable();
    errorList->addOnSelect(this, onErrorListSelect);
    errorList->addOnChange(this, onErrorListChange);

    size_t i = 0;
    for (const Adventure::ErrorLogEntry & error : adventure->getErrorLog())
        errorList->add(error.msg, reinterpret_cast<void*>(i++));

    infoBox = new LimitedTextBox(getTextDisplay(), 
        ivec2(2, getTextDisplay().getHeight() - 10), 
        getTextDisplay().getWidth() - 4, 
        8);
    infoBox->setInstant(true);

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
    errorList->pressKey(key);

    switch (key)
    {
    case VK_ESCAPE:
        getControler().changeDisplayer(Controler::dtAdventureSelection);
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
    t->showDescription();
}
