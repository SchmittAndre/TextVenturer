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
        }
    }

    // bunch of testing:                                                  
    write(1, 14, "You are most certainly not standing in a forest, duh!");

    AsciiArt art;
    art.loadFromFile("data/AsciiArt/logo.txt");
    draw((width - art.getWidth()) / 2, 2, art);

    cursorx = 5;
    cursory = 16;

    write(3, cursory, ">");
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

void TextDisplay::write(int x, int y, string str)
{
    for (DWORD p = 0; p < str.length(); p++)
        text[x + p][y]->setChar(str[p]);
}

void TextDisplay::draw(int x, int y, AsciiArt &art)
{
    for (int line = 0; line < art.getHeight(); line++)
        write(x, y + line, art(line));
}

void TextDisplay::pressChar(byte c)
{
    if (c == VK_BACK)
    {
        if (cursorx > 5)
            text[cursorx--][cursory]->setChar(' ');
    }
    else if (c == VK_RETURN)
    {
        if (cursory < height - 3)
        {
            write(cursorx, cursory, " ");
            write(3, cursory, " ");
            cursorx = 5;
            cursory += 1;
            write(3, cursory, ">");
        }
    }
    else if (c == 127)
    {
        while (text[cursorx - 1][cursory]->getChar() == ' ' && cursorx > 5)
            pressChar(VK_BACK);
        while (text[cursorx - 1][cursory]->getChar() != ' ' && cursorx > 5)
            pressChar(VK_BACK);
    }
    else if (cursorx < width - 2)
        text[cursorx++][cursory]->setChar(c);
}

void TextDisplay::update(float deltaTime)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            text[x][y]->update(deltaTime);

    static float t = 0;
    t += deltaTime;
    if (fmod(t, 0.5) < 0.25)
        write(cursorx, cursory, "_");
    else
        write(cursorx, cursory, " ");
}

void TextDisplay::render()
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            text[x][y]->render();
    vao->render();
}
