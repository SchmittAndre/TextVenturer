#include "stdafx.h"

#include "Adventure.h"
#include "Controler.h"
#include "ListSelection.h"
#include "TextDisplay.h"

#include "AdventureErrorLog.h"

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

    errorList = new ListSelection(getTextDisplay(), ivec2(2, 1), getTextDisplay().getWidth() - 4, getTextDisplay().getHeight() - 10);
    errorList->enable();
    errorList->addOnSelect(this, onErrorListSelect);
    errorList->addOnChange(this, onErrorListChange);

    for (Adventure::ErrorLogEntry error : adventure->getErrorLog())
        errorList->add(error.msg, &error);
}

void AdventureErrorLog::notifyUnload()
{
    adventure = NULL;

    delete errorList;
    errorList = NULL;
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
}
