#include "stdafx.h"   
#include "DisplayChar.h"
#include "AsciiArt.h"

#include "TextDisplay.h"

TextDisplay::TextDisplay(Shader* textShader, BMPFont* font, size_t width, size_t height)
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
    result.y = 1 - (2.0f * pos.y + 1) / height;   
    return result;
}

void TextDisplay::write(int x, int y, const string & str)
{
    for (size_t p = 0; p < str.length(); p++)
        write(x + p, y, str[p]);
}

void TextDisplay::write(ivec2 p, const string & str)
{
    write(p.x, p.y, str);
}

void TextDisplay::write(int x, int y, const byte c)
{
    if (isVisible(x, y))
        text[x][y]->setChar(c);
}

void TextDisplay::write(ivec2 p, const byte c)
{
    write(p.x, p.y, c);
}

void TextDisplay::draw(int x, int y, const AsciiArt & art)
{
    for (size_t line = 0; line < art.getHeight(); line++)
        write(x, y + line, art[line]);
}

void TextDisplay::draw(ivec2 p, const AsciiArt & art)
{
    draw(p.x, p.y, art);
}

void TextDisplay::move(ivec2 src, uvec2 size, ivec2 dest)
{
    for (size_t x = 0; x < size.x; x++)
        for (size_t y = 0; y < size.y; y++)
        {
            ivec2 s(src.x + x, src.y + y);
            ivec2 d(dest.x + x, dest.y + y);

            if (!isVisible(d))
                continue;

            if (!isVisible(s))
                text[d.x][d.y]->reset(true);
            else
                *(text[d.x][d.y]) = *(text[s.x][s.y]);                
        }
}

void TextDisplay::update(float deltaTime)
{
    for (size_t x = 0; x < width; x++)
        for (size_t y = 0; y < height; y++)
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
    for (size_t x = 0; x < width; x++)
        for (size_t y = 0; y < height; y++)
            text[x][y]->render();
    cursorChar->render();
    vao->render();
}

size_t TextDisplay::getWidth() const
{
    return width;
}

size_t TextDisplay::getHeight() const
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

bool TextDisplay::isColumnVisible(int x) const
{
    return x >= 0 && x < width;
}

bool TextDisplay::isLineVisible(int y) const
{
    return y >= 0 && y < height;
}

bool TextDisplay::isVisible(int x, int y) const
{
    return isColumnVisible(x) && isLineVisible(y);
}

bool TextDisplay::isVisible(ivec2 p) const
{
    return isVisible(p.x, p.y);
}

string TextDisplay::getLine(size_t y, size_t offset, size_t count) const
{
    string result;
    size_t end = count == string::npos ? width : offset + count;
    for (size_t x = offset; x < end; x++)
        result += getChar(x, y);
    return result;
}

byte TextDisplay::getChar(int x, int y) const
{
    return isVisible(x, y) ? text[x][y]->getChar() : ' ';
}

byte TextDisplay::getChar(ivec2 p) const
{
    return getChar(p.x, p.y);
}

void TextDisplay::clearLine(int y, size_t offset, size_t count)
{
    if (isLineVisible(y))
    {
        size_t end = count == string::npos ? width : offset + count;
        for (size_t x = offset; x < end; x++)
            text[x][y]->reset(true);
    }
}

void TextDisplay::clear()
{
	for (size_t x = 0; x < width; x++)
		for (size_t y = 0; y < height; y++)
			text[x][y]->reset(true);
}
