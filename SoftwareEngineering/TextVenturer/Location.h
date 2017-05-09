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
        stringlist prepAliasesList;    // stringlist for list/put          
        stringlist prepAliasesTake;    // stringlist also useable for take
        std::optional<Inventory> filter;
        Filter mode;
    public:
        PInventory(FileStream & stream, ref_vector<AdventureObject> & objectList);
        PInventory();

        bool addPrepositionAlias(std::string alias, bool runOnTake = false);
        bool delPrepositionAlias(std::string alias);
        std::string getPrepositionName(bool runOnTake = false, bool startOfSentence = false) const;
        bool hasPrepositionAlias(std::string alias, bool runOnTake = false) const;

        void addItem(Item & item);
        bool canAddItem(Item & item) const;
        void addItemForce(Item & item);

        bool isFiltered() const;
        Filter getFilterMode() const;
        void enableFilter(Filter mode);
        void disableFilter();
        void addToFilter(Item & item);
        void delFromFilter(Item & item);

        void save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs) const;
    };

private:
    std::unordered_map<std::string, PInventory> inventories;

    CommandArray* locatedCommands;

    CustomAdventureAction* onGoto;
    CustomAdventureAction* onLeave;

public:
    Location();
    virtual ~Location();

    PInventory & addInventory(std::string preposition);
    void delInventory(std::string preposition);
    bool hasInventory(std::string preposition) const;
    size_t filledInventoryCount() const;
    PInventory & firstFilledInventory() const;
    // std::vector<PInventory*> getInventories() const;

    CommandArray* getLocatedCommands() const;

    CustomAdventureAction* getOnGoto() const;
    CustomAdventureAction* getOnLeave() const;

    void setOnGoto(CustomAdventureAction* onGoto);
    void setOnLeave(CustomAdventureAction* onLeave);

    PInventory & getInventory(std::string preposition);

    std::string formatPrepositions(bool filledOnly = false) const;
    std::string formatPrepositions(Item & filterCheckItem) const;

    Type getType() const;
    void save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs, ref_idlist<CommandArray> & commandArrayIDs) const;
    void load(FileStream & stream, Adventure & adventure, ref_vector<AdventureObject> & objectList, ref_vector<CommandArray> & commandArrayList);
};

class EPrepositionNotFound : public Exception
{
public:
    EPrepositionNotFound(const Location* location, std::string preposition);
};

class EAddItemFilterForbidden : public Exception
{
public:
    EAddItemFilterForbidden(Item* item);
};