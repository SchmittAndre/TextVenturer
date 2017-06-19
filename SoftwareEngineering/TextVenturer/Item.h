#pragma once

#include "AdventureObject.h"

class CustomAdventureAction;

class Item : public AdventureObject
{
private:
    CommandArray carryCommands;

    CustomAdventureAction* onPlace;
    CustomAdventureAction* onTake;

public:        
    Item();
    Item(FileStream & stream, AdventureLoadHelp & help);
    ~Item();

    CustomAdventureAction* getOnTake() const;
    CustomAdventureAction* getOnPlace() const;

    void setOnTake(CustomAdventureAction* onTake);
    void setOnPlace(CustomAdventureAction* onPlace);

    CommandArray & getCarryCommands();

    Type getType() const;
    void save(FileStream & stream, AdventureSaveHelp & help) const;
};