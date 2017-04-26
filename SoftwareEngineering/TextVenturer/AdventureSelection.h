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

        void setState(State state);

    public:
        NamedAdventure(std::wstring filename);
        ~NamedAdventure();
        
        std::string getNameAnsi();

        FileType getFileType();
        std::string getDisplayName();

        State getState();
        
        bool operator<(const NamedAdventure& other);

        Adventure* getAdventure();
        
        void unloadAdventure();
        void loadAdventure(); 

        void addOnStateChanged(void* self, EventFuncNotify func);
        void delOnStateChanged(void* self, EventFuncNotify func);
    };

    enum Action 
    {
        acPlay,
        acCompile,
        acRename,
        acTextEditor,
        acWinExplorer,
        acDelete,

        ACTION_COUNT
    };

    std::vector<NamedAdventure> adventures;
    UINT selected;

    LineInput* searchBar;
    ListSelection* adventureSelection;
    ListSelection* actionSelection;
    TextBox* infoBox;

    std::mutex infoBoxSection;
    
    bool reloadList;

    void loadAdventures();

    friend static void onSearchBarChanged(void* self, void* sender);
    friend static void onAdventureSelect(void* self, void* sender);
    friend static void onAdventureSelectionChange(void* self, void* sender);
    friend static void onActionSelect(void* self, void* sender);
    friend static void onAdventureStateChanged(void* self, void* sender);

    void infoBoxLoading();
    void infoBoxError();
    void infoBoxDescription();

public:
    AdventureSelection(Controler* controler);
    ~AdventureSelection();

    void notifyLoad();
    void notifyUnload();

    void update(float deltaTime);

    void pressChar(byte c);
    void pressKey(byte key);

    const static std::string actionStrings[ACTION_COUNT];
};