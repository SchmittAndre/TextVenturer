#pragma once

#include "BaseGame.h"

class TextDisplay;
class Controler;
class GLWindow;

class Game : public BaseGame
{
private:
    TextDisplay* textDisplay;
    Shader* textShader;
    Controler* controler;
    BMPFont* font;
	GLWindow* window;
                                                                     
    float deltaTime;
    LARGE_INTEGER lastTime, frequency;

    void updateDeltaTime();

public:
    Game(GLWindow* w);
    virtual ~Game();

    void update();
    void render() const;
    void resize(int width, int height);
    void pressChar(byte c) const;
    void pressKey(byte key) const;

	GLWindow * getWindow() const;

};

