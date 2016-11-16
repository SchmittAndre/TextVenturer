#pragma once

#include "DisplayChar.h"
#include "AsciiArt.h"

class TextDisplay
{
private:
    VAO* vao;

    DisplayChar*** text;

    BMPFont* font;

    int width;
    int height;

    int cursorx;
    int cursory;

public:
    TextDisplay(Shader* textShader, BMPFont* font, int width, int height);
    ~TextDisplay();

    void write(int x, int y, string str);
    void draw(int x, int y, AsciiArt &art);

    void pressChar(byte c);

    void update(float deltaTime);
    void render();
};

