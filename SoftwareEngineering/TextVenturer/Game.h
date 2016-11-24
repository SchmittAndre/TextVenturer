#pragma once

#include "BaseGame.h"

class TextDisplay;
class Controler;
class Player;

class Game : public BaseGame
{
private:
    TextDisplay* textDisplay;
    Shader* textShader;
    Controler* controler;
	Player* player;
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
    void pressChar(byte c);

	Player * getPlayer();

};

