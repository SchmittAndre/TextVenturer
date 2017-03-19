#pragma once

#include "AdventureObject.h"

class CustomAdventureAction;

class Item : public AdventureObject
{
private:
    CommandArray* carryCommands;

    CustomAdventureAction* onTake;
    CustomAdventureAction* onPlace;

public:        
    Item();
    ~Item();

    CustomAdventureAction* getOnTake();
    CustomAdventureAction* getOnPlace();

    void setOnTake(CustomAdventureAction* onTake);
    void setOnPlace(CustomAdventureAction* onPlace);

    CommandArray* getCarryCommands();

    Type getType();
    void save(FileStream & stream, idlist<AdventureObject*> & objectIDs, idlist<CommandArray*> & commandArrayIDs);
    void load(FileStream & stream, Adventure * adventure, std::vector<AdventureObject*>& objectList, std::vector<CommandArray*>& commandArrayList);
};