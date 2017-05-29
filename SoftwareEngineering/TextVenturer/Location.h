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

    class MultiInventory : public Inventory
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
        Inventory filter;
        Filter mode;
    public:
        MultiInventory(FileStream & stream, ref_vector<AdventureObject> & objectList);
        MultiInventory();

        bool addPrepositionAlias(std::string alias, bool runOnTake = false);
        bool delPrepositionAlias(std::string alias);
        std::string getPrepositionName(bool runOnTake = false, bool startOfSentence = false) const;
        bool hasPrepositionAlias(std::string alias, bool runOnTake = false) const;

        void addItem(Item & item);
        bool canAddItem(Item & item) const;
        void addItemForce(Item & item);

        Filter getFilterMode() const;
        void enableFilter(Filter mode);
        void disableFilter();
        void addToFilter(Item & item);
        void delFromFilter(Item & item);

        void save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs) const;
    };

private:
    std::unordered_map<std::string, MultiInventory> inventories;

    CommandArray locatedCommands;

    CustomAdventureAction * onGoto;
    CustomAdventureAction * onLeave;

public:
    Location();
    Location(FileStream & stream, Adventure & adventure, ref_vector<AdventureObject> & objectList, ref_vector<CommandArray> & commandArrayList);
    ~Location();

    MultiInventory & addInventory(std::string preposition);
    void delInventory(std::string preposition);
    bool hasInventory(std::string preposition) const;
    size_t filledInventoryCount() const;
    MultiInventory & firstFilledInventory() const;

    CommandArray & getLocatedCommands();

    CustomAdventureAction * getOnGoto() const;
    CustomAdventureAction * getOnLeave() const;

    void setOnGoto(CustomAdventureAction * onGoto);
    void setOnLeave(CustomAdventureAction * onLeave);

    MultiInventory & getInventory(std::string preposition);

    std::string formatPrepositions(bool filledOnly = false) const;
    std::string formatPrepositions(Item & filterCheckItem) const;

    Type getType() const;
    void save(FileStream & stream, ref_idlist<AdventureObject> & objectIDs, ref_idlist<CommandArray> & commandArrayIDs) const;
};

class EPrepositionExistsAlready : public Exception
{
public:
    EPrepositionExistsAlready(const Location & location, const std::string & preposition);
};

class EPrepositionNotFound : public Exception
{
public:
    EPrepositionNotFound(const Location & location, const std::string & preposition);
};

class EAddItemFilterForbidden : public Exception
{
public:
    EAddItemFilterForbidden(const Item & item);
};

class EMultiInventoryEmpty : public Exception
{
public:
    EMultiInventoryEmpty(const Location & location);
};