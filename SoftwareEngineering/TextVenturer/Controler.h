#pragma once

class Adventure;   
class TextDisplay;
class GameDisplayer;
class MainMenu;
class OptionMenu;
class CmdLine;
class Game;

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

    Game* game;

    // Game Displayer
    MainMenu* mainMenu;
    OptionMenu* optionMenu;
    CmdLine* cmdLine;

    // current Displayer
    GameDisplayer* currentDisplayer;

public:
    Controler(Game* game, TextDisplay* textDisplay);
    virtual ~Controler();

    TextDisplay* getTextDisplay();
    Game* getGame();

    void pressChar(byte c);
    void pressKey(byte key);

    void update(float deltaTime);

    void changeDisplayer(DisplayerType type);
    
    bool loadAdventure(std::string filename);
    bool loadAdventureState(std::string filename);
    bool saveAdventureState(std::string filename);
    void startAdventure();

    void unloadAdventure();
};

