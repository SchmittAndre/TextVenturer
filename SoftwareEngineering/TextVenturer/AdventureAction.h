#pragma once

class Adventure;

#include "BaseAction.h"

class AdventureAction : public BaseAction
{
private:
    Adventure* adventure;
public:
    AdventureAction(Adventure* adventure);
    Adventure* getAdventure() const;
};

