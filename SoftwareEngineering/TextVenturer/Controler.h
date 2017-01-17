#pragma once

class CommandSystem;
class DefaultAction;
class Adventure;
class TextDisplay;
class Game;

class Controler
{
private:                          
    CommandSystem* commandSystem;

    DefaultAction* defaultAction;

    Adventure* adventure;

    TextDisplay* textDisplay;
	Game* game;
    int cursorMin;
    int cursorMax;

    TextDisplay::State state;
    size_t writepos;
    bool newLine;
    queue<string> textbuffer;

    string input;

    void updateInput();

public:
    Controler(TextDisplay* textDisplay, Game* game);
    virtual ~Controler();

    void pressChar(byte c);

    void update(float deltaTime);

    void writeLine(string msg, TextDisplay::State & state = TextDisplay::State());

    void command(string msg);
    
    void DEBUG_startAdventure();

};

