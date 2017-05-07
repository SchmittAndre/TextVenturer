#pragma once

#include "AliasList.h"

class Adventure;
class Player;                  
class CustomAdventureAction;
class CommandArray;

class AdventureObject abstract
{
public:
    enum Type
    {
        otRoom,
        otLocation,
        otRoomConnection,
        otItem
    };  
private:
    AliasList aliases;
    std::string description;    

    CustomAdventureAction* onInspect;

    taglist flags;

protected:
    static void saveAdventureAction(FileStream & stream, CustomAdventureAction* action);
    static void loadAdventureAction(FileStream & stream, Adventure & adventure, CustomAdventureAction*& action);

public:
    AdventureObject();
    AdventureObject();
    virtual ~AdventureObject();

    AliasList& getAliases();
    
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player & player, bool startOfSentence = false) const;
    std::string getNameOnly(bool startOfSentence = false) const;
    bool isNamePlural() const;
    void setDescription(std::string description);
    std::string getDescription() const;

    CustomAdventureAction* getOnInspect() const;
    void setOnInspect(CustomAdventureAction* onInspect);

    void setFlag(std::string flag);
    void clearFlag(std::string flag);
    void toggleFlag(std::string flag);
    bool testFlag(std::string flag) const;
    
    virtual Type getType() const = 0;
    virtual void save(FileStream & stream, idlist<AdventureObject&> & objectIDs, idlist<CommandArray*> & commandArrayIDs) const;
    virtual void load(FileStream & stream, Adventure * adventure, std::vector<AdventureObject*>& objectList, std::vector<CommandArray*>& commandArrayList);
};

