#pragma once

#include "TextDisplay.h"
#include "BaseGame.h"

class Game : public BaseGame
{
private:
    TextDisplay* textDisplay;
    Shader* textShader;

    BMPFont* font;

    float deltaTime;
    LARGE_INTEGER lastTime, frequency;

    void updateDeltaTime();

public:
    Game();
    ~Game();

    void update();
    void render();
    void resize(int width, int height);

};

