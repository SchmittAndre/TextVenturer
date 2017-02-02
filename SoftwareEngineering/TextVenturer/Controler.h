#pragma once

#include "Game.h"
#include "CommandSystem.h"
#include "DefaultAction.h"

class Adventure;
class CommandSystem;
class Game;

class Controler
{
private:                          
    CommandSystem* commandSystem;

    DefaultAction* defaultAction;

    Adventure* adventure;

    TextDisplay* textDisplay;
	Game* game;

    TextDisplay::State state;
    int writepos;
    bool newLine;
    queue<string> textbuffer;

    string input;
    size_t inputPos;
    size_t inputScroll;

    vector<string> inputHistory;
    bool msgSaved;
    size_t historyIndex;

    void updateInput();
    void writeToBuffer(string msg);

public:
    Controler(TextDisplay* textDisplay, Game* game);
    virtual ~Controler();

    void pressChar(byte c);
    void pressKey(byte key);

    void update(float deltaTime);

    void write(string msg);

    void sendCommand(string msg);
    
    void DEBUG_startAdventure();

};

