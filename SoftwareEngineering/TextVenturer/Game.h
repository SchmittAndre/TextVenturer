#pragma once
#include "BaseGame.h"
class Game : public BaseGame
{
private:
    VAO* testvao;
    Shader* shader;

public:
    Game();
    ~Game();

    void update();
    void render();
};

