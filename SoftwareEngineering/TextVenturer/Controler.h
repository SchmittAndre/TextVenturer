#pragma once

class Adventure;   
class TextDisplay;
class GameDisplayer;
class MainMenu;
class OptionMenu;
class CmdLine;
class Game;
class AdventureSelection;

class Controler
{
public:
    enum DisplayerType
    {
         dtMainMenu,
         dtAdventureSelection,
         dtOptionMenu,
         dtAdventure
    };

private:                          
    TextDisplay* textDisplay;

    Game* game;

    Adventure* adventure; // TODO: remove this

    // Game Displayer
    MainMenu* mainMenu;
    AdventureSelection* adventureSelection;
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
};

