#include "stdafx.h"   
#include "DisplayChar.h"
#include "AsciiArt.h"

#include "TextDisplay.h"

TextDisplay::TextDisplay(Shader* textShader, BMPFont* font, int width, int height)
{
    vao = new VAO(textShader);
    vao->generate((width * height + 1) * 6, buStreamDraw);
    vao->forceMaxSize();

    this->font = font;
    font->uniform(textShader, "font");

    this->width = width;
    this->height = height;
    text = new DisplayChar**[width];
    vec2 pos;
    float scale = 1.0f / height;
    for (int x = 0; x < width; x++)
    {
        text[x] = new DisplayChar*[height];
        for (int y = 0; y < height; y++)
        {
            pos = getCharPos(ivec2(x, y));
            text[x][y] = new DisplayChar(vao, font, (y * width + x) * 6, pos, scale);
        }
    }

    cursorChar = new DisplayChar(vao, font, width * height * 6, vec2(0, 0), scale);

    cursorVisible = true;
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
    delete cursorChar;
}

vec2 TextDisplay::getCharPos(ivec2 pos) const
{
    vec2 result;
    result.x = ((2.0f * pos.x - width + 1) / height) * DisplayChar::pixelAspect;
    result.y = 1 - (2.0f * pos.y - 1) / height;
    /*
    result.x = ((2.0f * pos.x - width) / height + 1.0f / height) * DisplayChar::pixelAspect;
    result.y = 1 - 2.0f * pos.y / height - 1.0f / height;
    */    
    return result;
}

void TextDisplay::write(int x, int y, const string & str)
{
    for (DWORD p = 0; p < str.length(); p++)
        text[x + p][y]->setChar(str[p]);
}

void TextDisplay::write(ivec2 p, const string & str)
{
    write(p.x, p.y, str);
}

void TextDisplay::write(int x, int y, const byte c)
{
    text[x][y]->setChar(c);
}

void TextDisplay::write(ivec2 p, const byte c)
{
    write(p.x, p.y, c);
}

void TextDisplay::draw(int x, int y, const AsciiArt & art)
{
    for (int line = 0; line < art.getHeight(); line++)
        write(x, y + line, art[line]);
}

void TextDisplay::draw(ivec2 p, const AsciiArt & art)
{
    draw(p.x, p.y, art);
}

void TextDisplay::update(float deltaTime)
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            text[x][y]->update(deltaTime);

    cursorTime -= deltaTime;
    if (cursorTime <= 0)
        resetCursorTime();

    if (cursorVisible && cursorTime > 0.5)
        cursorChar->setChar('_');
    else
        cursorChar->setChar(' ');

    cursorChar->update(deltaTime);
}

void TextDisplay::render()
{
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++)
            text[x][y]->render();
    cursorChar->render();
    vao->render();
}

int TextDisplay::getWidth() const
{
    return width;
}

int TextDisplay::getHeight() const
{
    return height;
}

void TextDisplay::setCursorVisible(bool visible)
{
    cursorVisible = visible;
}

bool TextDisplay::getCursorVisible() const
{
    return cursorVisible;
}

void TextDisplay::resetCursorTime()
{
    cursorTime = 1;
}

void TextDisplay::setCursorPos(ivec2 pos)
{
    cursorPos = pos;
    cursorChar->setPos(getCharPos(pos));
}

ivec2 TextDisplay::getCursorPos() const
{
    return cursorPos;
}

string TextDisplay::getLine(int y) const
{
    string temp;
    for (int i = 0; i < (this->width); i++)
    {
        temp += getChar(i, y);
    }
    return temp;
}

byte TextDisplay::getChar(int x, int y) const
{
    return text[x][y]->getChar();
}

byte TextDisplay::getChar(ivec2 p) const
{
    return getChar(p.x, p.y);
}

void TextDisplay::clearline(int y)
{
	for (int i = 0; i <width;i++)
	{
		text[i][y]->setChar(' ');
	}        		
}

void TextDisplay::clear()
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
			text[x][y]->setChar(' ');
}
