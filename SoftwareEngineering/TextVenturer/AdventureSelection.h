#pragma once

#include "GameDisplayer.h"

class Adventure;
class LineInput;
class ListSelection;
class TextBox;

class AdventureSelection : public GameDisplayer
{
private:    
    
    class NamedAdventure
    {
    public:
        enum State
        {
            stNotLoaded,
            stLoading,
            stLoadSuccess,
            stLoadFailure
        };

        enum FileType
        {
            ftUnknown,
            ftScript,
            ftCompiled
        };
    private:
        std::wstring filename;
        Adventure* adventure;
        
        State state;

        NotifyEvent onStateChanged;

        std::mutex loadingSection;

        void setState(State state);

    public:
        NamedAdventure(std::wstring filename);
        ~NamedAdventure();
        
        std::string getNameAnsi() const;
        std::wstring getName() const;

        FileType getFileType() const;
        std::string getDisplayName() const;

        State getState() const;
        
        static bool compare(const NamedAdventure* a, const NamedAdventure* b);

        Adventure* getAdventure() const;
        Adventure* getAdventureOwnership();
        
        void unloadAdventure();
        void loadAdventure(); 

        void addOnStateChanged(void* self, EventFuncNotify func);
        void delOnStateChanged(void* self, EventFuncNotify func);
    };

    class ActionBase abstract
    {
    private:
        AdventureSelection* adventureSelection;          

    public:
        ActionBase(AdventureSelection* adventureSelection);  
        virtual ~ActionBase();

        AdventureSelection* getAdventureSelection();
        virtual bool canExecute(NamedAdventure* adventure) = 0;
        virtual void execute(NamedAdventure* adventure);
        virtual std::string getDisplayString() = 0;
    };

    class ActionPlay : public ActionBase
    {
    public:
        ActionPlay(AdventureSelection* adventureSelection) : ActionBase(adventureSelection) {};
        bool canExecute(NamedAdventure* adventure);
        void execute(NamedAdventure* adventure);
        std::string getDisplayString();
    };

    class ActionErrorLog : public ActionBase
    {
    public:
        ActionErrorLog(AdventureSelection* adventureSelection) : ActionBase(adventureSelection) {};
        bool canExecute(NamedAdventure* adventure);
        //void execute(NamedAdventure* adventure);
        std::string getDisplayString();
    };

    class ActionCompile : public ActionBase
    {
    public:
        ActionCompile(AdventureSelection* adventureSelection) : ActionBase(adventureSelection) {};
        bool canExecute(NamedAdventure* adventure);
        //void execute(NamedAdventure* adventure);
        std::string getDisplayString();
    };

    class ActionRename : public ActionBase
    {
    public:
        ActionRename(AdventureSelection* adventureSelection) : ActionBase(adventureSelection) {};
        bool canExecute(NamedAdventure* adventure);
        //void execute(NamedAdventure* adventure);
        std::string getDisplayString();
    };

    class ActionTextEditor : public ActionBase
    {
    public:
        ActionTextEditor(AdventureSelection* adventureSelection) : ActionBase(adventureSelection) {};
        bool canExecute(NamedAdventure* adventure);
        //void execute(NamedAdventure* adventure);
        std::string getDisplayString();
    };

    class ActionWinExplorer : public ActionBase
    {
    public:
        ActionWinExplorer(AdventureSelection* adventureSelection) : ActionBase(adventureSelection) {};
        bool canExecute(NamedAdventure* adventure);
        //void execute(NamedAdventure* adventure);
        std::string getDisplayString();
    };

    class ActionDelete : public ActionBase
    {
    public:
        ActionDelete(AdventureSelection* adventureSelection) : ActionBase(adventureSelection) {};
        bool canExecute(NamedAdventure* adventure);
        //void execute(NamedAdventure* adventure);
        std::string getDisplayString();
    };

    std::vector<NamedAdventure*> adventures;
    std::vector<ActionBase*> actions;
    UINT selected;

    LineInput* searchBar;
    ListSelection* adventureSelection;
    ListSelection* actionSelection;
    TextBox* infoBox;

    std::mutex infoBoxSection;

    float actionsTimer;
    bool actionsVisible;
    bool regenList;

    void loadAdventures();   
    void unloadAdventures();
    void generateList();

    friend static void onSearchBarChanged(void* self, void* sender);
    friend static void onAdventureSelect(void* self, void* sender);
    friend static void onAdventureSelectionChange(void* self, void* sender);
    friend static void onActionSelect(void* self, void* sender);
    friend static void onAdventureStateChanged(void* self, void* sender);

    void infoBoxLoading();
    void infoBoxError();
    void infoBoxDescription();
    void infoBoxNoAdventure();

    void updateSelectedAdventure();

public:
    AdventureSelection(Controler* controler);
    ~AdventureSelection();

    void notifyLoad();
    void notifyUnload();

    void update(float deltaTime);

    void pressChar(byte c);
    void pressKey(byte key);
};