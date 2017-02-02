#pragma once

#include "TextDisplay.h"
#include "Window.h"
#include "Controler.h"
#include "BaseGame.h"

class Controler;

class Game : public BaseGame
{
private:
    TextDisplay* textDisplay;
    Shader* textShader;
    Controler* controler;
    BMPFont* font;
	GLWindow* window;
                                                                     
    float deltaTime;
    float fpsUpdate;
    float fps;
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

    float getRawFPS();

	GLWindow * getWindow() const;

};

