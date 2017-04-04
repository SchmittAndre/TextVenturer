#pragma once

class TextDisplay;
class Controler;

class GameDisplayer
{
private:
    Controler* controler;

protected:
    
    TextDisplay* getTextDisplay();

public:
    GameDisplayer(Controler* controler);
    ~GameDisplayer();

	Controler* getControler();

    virtual void pressChar(byte c) { };
    virtual void pressKey(byte key) { };

    virtual void update(float deltaTime) { };

    virtual void notifySwitch() { };
};