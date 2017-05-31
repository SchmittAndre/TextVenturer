#pragma once

#include "AliasList.h"

struct AdventureLoadHelp;
struct AdventureSaveHelp;
class Player;                  
class CustomAdventureAction;
class CommandArray;

class AdventureObject abstract
{
public:

    enum Type
    {
        otItem,
        otLocation,
        otRoom,
        otRoomConnection
    };

private:
    AliasList aliases;
    std::string description;

    taglist flags;

    CustomAdventureAction * onInspect;

public:

    AdventureObject(FileStream & stream, AdventureLoadHelp & help);
    AdventureObject();
    virtual ~AdventureObject();

    AliasList& getAliases();
    
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player & player, bool startOfSentence = false) const;
    std::string getNameOnly(bool startOfSentence = false) const;
    bool isNamePlural() const;
    void setDescription(std::string description);
    std::string getDescription() const;

    CustomAdventureAction * getOnInspect() const;
    void setOnInspect(CustomAdventureAction * onInspect);

    void setFlag(std::string flag);
    void clearFlag(std::string flag);
    void toggleFlag(std::string flag);
    bool testFlag(std::string flag) const;
    
    virtual Type getType() const = 0;
    virtual void save(FileStream & stream, AdventureSaveHelp & help) const;
};

