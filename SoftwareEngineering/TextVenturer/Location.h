#pragma once

#include "AdventureObject.h"
#include "Inventory.h"
#include "CommandSystem.h"

class Command;
class CustomAdventureAction;
class Item;
class Player;
class Room;

class Location : public AdventureObject
{
public:

    class PInventory : public Inventory
    {
    public:
        enum Filter
        {
            ifBlacklist,
            ifWhitelist
        };
    private:
        strings prepAliasesList;    // strings for list/put          
        strings prepAliasesTake;    // strings also useable for take
        Inventory* filter;
        Filter mode;
    public:
        PInventory(FileStream & stream, std::vector<AdventureObject*> & objectList);
        PInventory();
        ~PInventory();

        bool addPrepositionAlias(std::string alias, bool runOnTake = false);
        bool delPrepositionAlias(std::string alias);
        std::string getPrepositionName(bool runOnTake = false, bool startOfSentence = false) const;
        bool hasPrepositionAlias(std::string alias, bool runOnTake = false) const;

        bool addItem(Item* item);
        bool canAddItem(Item* item);
        void addItemForce(Item* item);

        bool isFiltered() const;
        Filter getFilterMode() const;
        void enableFilter(Filter mode);
        void disableFilter();
        void addToFilter(Item* item);
        bool delFromFilter(Item* item);

        void save(FileStream & stream, idlist<AdventureObject*> objectIDs);
    };

private:
    std::unordered_map<std::string, PInventory*> inventories;

    CommandArray* locatedCommands;

    CustomAdventureAction* onGoto;
    CustomAdventureAction* onLeave;

public:
    Location();
    virtual ~Location();

    PInventory* addInventory(std::string preposition);
    bool delInventory(std::string preposition);
    bool hasInventory(std::string preposition);
    size_t filledInventoryCount();
    PInventory* firstFilledInventory();
    std::vector<PInventory*> getInventories();

    CommandArray* getLocatedCommands();

    CustomAdventureAction* getOnGoto();
    CustomAdventureAction* getOnLeave();

    void setOnGoto(CustomAdventureAction* onGoto);
    void setOnLeave(CustomAdventureAction* onLeave);

    PInventory* getInventory(std::string preposition);

    std::string formatPrepositions(bool filledOnly = false);
    std::string formatPrepositions(Item* filterCheckItem);

    Type getType();
    void save(FileStream & stream, idlist<AdventureObject*> & objectIDs, idlist<CommandArray*> & commandArrayIDs);
    void load(FileStream & stream, Adventure * adventure, std::vector<AdventureObject*> & objectList, std::vector<CommandArray*>& commandArrayList);
};
