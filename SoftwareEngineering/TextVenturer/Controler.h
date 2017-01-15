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

    string input;

    void updateInput();

public:
    Controler(TextDisplay* textDisplay,Game* game);
    virtual ~Controler();

    void pressChar(byte c);

    void update(float deltaTime);

    void writeLine(string msg);

    void command(string msg) const;

};

