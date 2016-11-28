#pragma once

class TextDisplay;
class Game;

class Controler
{
private:
    TextDisplay* textDisplay;
	Game* game;
    int cursorMin;
    int cursorMax;

    string input;

    void updateInput();

public:
    Controler(TextDisplay* textDisplay,Game* game);

    void pressChar(byte c);

    void update(float deltaTime);

    void command(string msg);

};

