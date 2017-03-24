#pragma once

class Adventure;   
class TextDisplay;
class GameDisplayer;
class MainMenu;
class OptionMenu;
class CmdLine;

class Controler
{
public:
    enum DisplayerType
    {
         dtMainMenu,
         dtOptionMenu,
         dtAdventure
    };

private:                          
    Adventure* adventure;
    TextDisplay* textDisplay;

    // Game Displayer
    MainMenu* mainMenu;
    OptionMenu* optionMenu;
    CmdLine* cmdLine;

    // current Displayer
    GameDisplayer* currentDisplayer;

public:
    Controler(TextDisplay* textDisplay);
    virtual ~Controler();

    void pressChar(byte c);
    void pressKey(byte key);

    void update(float deltaTime);
    
    bool loadAdventure(std::string filename);
    bool loadAdventureState(std::string filename);
    bool saveAdventureState(std::string filename);
    void startAdventure();

    void unloadAdventure();
};

