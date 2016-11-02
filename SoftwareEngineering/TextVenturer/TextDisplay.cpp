#include "stdafx.h"

#include "TextDisplay.h"

TextDisplay::TextDisplay(Shader* textShader, BMPFont* font, int width, int height)
{
    vao = new VAO(textShader);
    vao->generate(width * height * 6, buStreamDraw);
    vao->forceMaxSize();

    this->font = font;
    font->uniform(textShader, "");

    this->width = width;
    this->height = height;
    text = new DisplayChar**[width];
    vec2 pos;
    float scale;
    for (int x = 0; x < width; x++)
    {
        text[x] = new DisplayChar*[height];
        for (int y = 0; y < height; y++)
        {
            scale = 1.0f / height;
            pos.x = ((2.0f * x - width) / height + scale) * DisplayChar::pixelAspect;
            pos.y = 1 - 2.0f * y / height - scale;

            text[x][y] = new DisplayChar(vao, font, (y * width + x) * 6, pos, scale);
            text[x][y]->setChar(rand() % 256);
            //text[x][y]->setChar(y * width + x);
            //text[x][y]->setAngularVelocity(((float)rand() / RAND_MAX) * 720 - 360);
            //text[x][y]->setVelocity(vec2(0.1f, 0).rotate(((float)rand() / RAND_MAX) * 360));
            //text[x][y]->setColor(Color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX));
            //text[x][y]->setGravity(true);
        }
    }
    
    text[1][1]->setChar('W');
    text[2][1]->setChar('h');
    text[3][1]->setChar('a');
    text[4][1]->setChar('t');
    text[6][1]->setChar('d');
    text[7][1]->setChar('o');
    text[9][1]->setChar('y');
    text[10][1]->setChar('o');
    text[11][1]->setChar('u');
    text[13][1]->setChar('w');
    text[14][1]->setChar('a');
    text[15][1]->setChar('n');
    text[16][1]->setChar('t');
    text[18][1]->setChar('t');
    text[19][1]->setChar('o');
    text[21][1]->setChar('d');
    text[22][1]->setChar('o');
    text[23][1]->setChar('?');
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

void TextDisplay::update(float deltaTime)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            text[x][y]->update(deltaTime);
}

void TextDisplay::render()
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            text[x][y]->render();
    vao->render();
}
