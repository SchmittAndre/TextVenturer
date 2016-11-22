#pragma once

class TextDisplay;

class Controler
{
private:
    TextDisplay* textDisplay;
    int cursorMin;
    int cursorMax;

    string input;

    void updateInput();

public:
    Controler(TextDisplay* textDisplay);

    void pressChar(byte c);

    void update(float deltaTime);

    void command(string msg);

};

