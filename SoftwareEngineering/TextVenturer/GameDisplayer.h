#pragma once

class TextDisplay;
class Controler;

class GameDisplayer abstract
{
private:
    Controler & controler;
    bool loaded;

protected:                            
    TextDisplay & getTextDisplay();

public:
    GameDisplayer(Controler & controler);
    virtual ~GameDisplayer();

	Controler & getControler();

    virtual void pressChar(byte c) { };
    virtual void pressKey(byte key) { };

    virtual void update(float deltaTime) { };

    virtual void notifyLoad();
    virtual void notifyUnload();

    bool active() const;
};