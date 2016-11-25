#pragma once

#include "BaseGame.h"

class TextDisplay;
class Controler;
class Player;
class GLWindow;

class Game : public BaseGame
{
private:
    TextDisplay* textDisplay;
    Shader* textShader;
    Controler* controler;
	Player* player;
    BMPFont* font;
	GLWindow* window;
                                                                     
    float deltaTime;
    LARGE_INTEGER lastTime, frequency;

    void updateDeltaTime();

public:
    Game(GLWindow* w);
    ~Game();

    void update();
    void render();
    void resize(int width, int height);
    void pressChar(byte c);

	Player * getPlayer();

	GLWindow * getWindow();

};

