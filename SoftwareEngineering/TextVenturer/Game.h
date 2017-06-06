#pragma once

#include "BaseGame.h"
#include "TextDisplay.h"
#include "Controler.h"

class GLWindow;

class Game : public BaseGame
{
private:
    VertexFragmentShader textShader;
    TextDisplay textDisplay;
    Controler controler;
    BMPFont font;
	GLWindow & window;

    float fpsUpdate;

    static const Shader::Attributes textShaderAttributes;

public:
    Game(GLWindow & window);

    void update(float deltaTime);
    void render();
    void resize(int width, int height);
    void pressChar(byte c);
    void pressKey(byte key);

	GLWindow & getWindow() const;
};

