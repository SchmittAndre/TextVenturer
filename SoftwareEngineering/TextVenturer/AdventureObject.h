#pragma once

class Player;                  
class CustomAdventureAction;
class CommandArray;

class AdventureObject abstract
{
protected:
    enum Type // only used for saving/loading, otherwise use dynamic_cast
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

    tags flags;

protected:
    static void saveAdventureAction(FileStream & stream, CustomAdventureAction* action);
    virtual Type getType() = 0;

public:
    AdventureObject();
    virtual ~AdventureObject();
    AliasList& getAliases();
    std::string getName(bool definiteArticle = false, bool startOfSentence = false) const;
    std::string getName(Player* player, bool startOfSentence = false) const;
    std::string getNameOnly(bool startOfSentence = false) const;
    bool isNamePlural() const;
    void setDescription(std::string description);
    std::string getDescription() const;

    CustomAdventureAction* getOnInspect();
    void setOnInspect(CustomAdventureAction* onInspect);

    void setFlag(std::string flag);
    void clearFlag(std::string flag);
    bool testFlag(std::string flag);      
    
    virtual void save(FileStream & stream, idlist<AdventureObject*> & objectIDs, idlist<CommandArray*> & commandArrayIDs);
};

