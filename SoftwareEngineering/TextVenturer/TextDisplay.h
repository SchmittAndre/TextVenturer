#pragma once
#include "DisplayChar.h"
class TextDisplay
{
private:
    VAO* vao;

    DisplayChar*** text;

    BMPFont* font;

    int width;
    int height;

public:
    TextDisplay(Shader* textShader, BMPFont* font, int width, int height);
    ~TextDisplay();

    void update(float deltaTime);
    void render();
};

