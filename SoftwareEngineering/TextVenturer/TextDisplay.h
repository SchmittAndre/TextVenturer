#pragma once
#include "DisplayChar.h"
class TextDisplay
{
private:
    VAO* vao;

    DisplayChar*** text;

    int width;
    int height;

public:
    TextDisplay(Shader* textShader, int width, int height);
    ~TextDisplay();

    void update();
    void render();
};

