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

    std::vector<NamedAdventure*> adventures;
    UINT selected;

    LineInput* searchBar;
    ListSelection* adventureSelection;
    ListSelection* actionSelection;
    TextBox* infoBox;

    std::mutex infoBoxSection;
    
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

    const static std::string actionStrings[ACTION_COUNT];
};