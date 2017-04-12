#pragma once

#include "GameDisplayer.h"

class Adventure;
class LineInput;
class ListSelection;

class AdventureSelection : public GameDisplayer
{
private:
    struct NamedAdventure
    {
        std::wstring filename;
        Adventure* adventure;

        std::string getDisplayName();

        bool operator<(const NamedAdventure& other);
        
        NamedAdventure(std::wstring filename);
    };

    std::vector<NamedAdventure> adventures;
    UINT selected;

    LineInput* searchBar;
    ListSelection* advSelection;
    
    bool listChanged;

    void unloadAdventures();
    void loadAdventures();

public:
    AdventureSelection(Controler* controler);
    ~AdventureSelection();

    void notifySwitch();

    void update(float deltaTime);

    void pressChar(byte c);
    void pressKey(byte key);
};