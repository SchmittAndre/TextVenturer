#include "stdafx.h"

#include "TextDisplay.h"

TextDisplay::TextDisplay(Shader* textShader, int width, int height)
{
    vao = new VAO(textShader);
    vao->generate(width * height * 6, buStreamDraw);
    vao->forceSize(6);

    this->width = width;
    this->height = height;
    text = new DisplayChar**[width];
    for (int x = 0; x < width; x++)
    {
        text[x] = new DisplayChar*[height];
        for (int y = 0; y < height; y++)                                 
            text[x][y] = new DisplayChar(vao, (y * width + x) * 6);        
    }
}

TextDisplay::~TextDisplay()
{
    delete vao;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
            delete text[x][y];
        delete[] text[x];
    }
    delete[] text;
}

void TextDisplay::update()
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            text[x][y]->update();
}

void TextDisplay::render()
{
    vao->render();
}
