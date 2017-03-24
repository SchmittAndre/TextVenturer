#pragma once

class TextDisplay;

class GameDisplayer
{
private:
    TextDisplay* textDisplay;

protected:
    TextDisplay* getTextDisplay();

public:
    GameDisplayer(TextDisplay* textDisplay);
    ~GameDisplayer();

    virtual void pressChar(byte c) { };
    virtual void pressKey(byte key) { };

    virtual void update(float deltaTime) { };

    virtual void notifySwitch() { };
};