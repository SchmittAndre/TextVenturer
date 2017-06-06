#pragma once

#include "GameDisplayer.h"

class Adventure;

class AdventureErrorLog : public GameDisplayer
{
private:
    Adventure * adventure;

public:                          
    AdventureErrorLog(Controler & controler);

    void setAdventure(Adventure & adventure);

    void notifyLoad();
    void notifyUnload();
};