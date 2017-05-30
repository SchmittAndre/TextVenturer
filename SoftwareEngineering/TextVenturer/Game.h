#pragma once

#include "BaseGame.h"
#include "TextDisplay.h"
#include "Controler.h"

class GLWindow;

class Game : public BaseGame
{
private:
    TextDisplay textDisplay;
    Shader textShader;
    Controler controler;
    BMPFont font;
	GLWindow & window;
                                                                     
    float deltaTime;
    float fpsUpdate;
    float fps;
    LARGE_INTEGER lastTime, frequency;

    void updateDeltaTime();

public:
    Game(GLWindow & window);

    void update();
    void render();
    void resize(int width, int height);
    void pressChar(byte c);
    void pressKey(byte key);

    float getRawFPS() const;

	GLWindow & getWindow() const;
};

