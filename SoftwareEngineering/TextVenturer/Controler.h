#pragma once

#include "TextDisplay.h"

class CommandSystem;
class DefaultAction;
class Adventure;
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
    std::queue<std::string> textbuffer;

    std::string input;
    size_t inputPos;
    size_t inputScroll;

  std::vector<std::string> inputHistory;
    bool msgSaved;
    size_t historyIndex;

    void updateInput();
    void writeToBuffer(std::string msg);

public:
    Controler(TextDisplay* textDisplay, Game* game);
    virtual ~Controler();

    void pressChar(byte c);
    void pressKey(byte key);

    void update(float deltaTime);

    void write(std::string msg);

    void sendCommand(std::string msg);
    
    bool loadAdventure(std::string filename);
    void DEBUG_startAdventure();

};

