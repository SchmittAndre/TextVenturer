#pragma once

class Adventure;   
class TextDisplay;
class GameDisplayer;
class MainMenu;
class OptionMenu;
class CmdLine;
class Game;
class AdventureSelection;
class AdventureErrorLog;

class Controler
{
public:
    enum DisplayerType
    {
         dtMainMenu,
         dtAdventureSelection,
         dtErrorLog,
         dtOptionMenu,
         dtAdventure,

         DISPLAYER_TYPE_COUNT
    };

private:                          
    TextDisplay & textDisplay;

    Game & game;

    GameDisplayer* displayer[DISPLAYER_TYPE_COUNT];
    DisplayerType currentDisplayer;
    DisplayerType nextDisplayer;

public:
    Controler(Game & game, TextDisplay & textDisplay);
    virtual ~Controler();

    TextDisplay & getTextDisplay() const;
    Game & getGame() const;

    void pressChar(byte c);
    void pressKey(byte key);

    void update(float deltaTime);

    void changeDisplayer(DisplayerType type);
    DisplayerType getCurrentDisplayerType() const;
    GameDisplayer & getCurrentDisplayer() const;
    GameDisplayer & getDisplayer(DisplayerType type) const;

    CmdLine & getCmdLine() const;
    AdventureErrorLog & getErrorLog() const;
};

