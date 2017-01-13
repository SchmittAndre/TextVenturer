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
    

    Adventure* adventure;

    TextDisplay* textDisplay;
	Game* game;
    int cursorMin;
    int cursorMax;

    string input;

    void updateInput();

public:
    Controler(TextDisplay* textDisplay,Game* game);
    ~Controler();

    void pressChar(byte c);

    void update(float deltaTime);

    void textscrolling(string msg);

    void command(string msg);

};

