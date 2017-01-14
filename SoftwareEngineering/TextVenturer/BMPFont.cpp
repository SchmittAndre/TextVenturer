#include "stdafx.h"
#include "Texture.h"

#include "BMPFont.h"

void BMPFont::generateWidths()
{
    int c;
    bool next;

    for (c = 0; c < 256; c++)
    {
        //if (c == 32)
        //    continue;
        next = false;
        for (int x = size - 1; x >= 0; x--)
        {
            for (int y = 0; y < size; y++)
                if (getPixel(c, x, y).a)
                {
                    widths[c] = (x * 8.0f + 8) / 256;
                    next = true;
                    break;
                }
            if (next)
                break;
        }
    }
    //widths[32] = 0.25;  // Normal Space
    //widths[160] = 0.25; // Non-Breaking Space
}

BMPFont::~BMPFont()
{
    if (data.getTexture())
        delete data.getTexture();     
}

void BMPFont::loadFromPNG(string filename)
{
    data.setTexture(new TextureData(filename));
    size = data.getTexture()->getSize().x / 16;
    generateWidths();
}

Color BMPFont::getPixel(byte c, int x, int y) const
{
    //int offset = (((c % 16) + (15 - c / 16) * 16 * size + size - 1 - y * size) * size + x) * 4;
    int offset = ((c % 16) + (15 - c / 16) * 256 + (15 - y) * 16) * 64 + x * 4;
    Color result;
    result.r = (float)(data.getTexture()->getData() + offset)[0] / 0xFF;
    result.g = (float)(data.getTexture()->getData() + offset)[1] / 0xFF;
    result.b = (float)(data.getTexture()->getData() + offset)[2] / 0xFF;
    result.a = (float)(data.getTexture()->getData() + offset)[3] / 0xFF;
    return result;
}

float BMPFont::getWidth(byte c) const
{
    return widths[c];
}

void BMPFont::uniform(Shader * shader, string name) const
{
    data.uniform(shader, name);
}

vec2 BMPFont::getTexCoord(byte c, vec2 texcoord) const
{
    vec2 result;
    result.x = (c % 16) / 16.0f + texcoord.x / 16;
    result.y = (15 - c / 16) / 16.0f + texcoord.y / 16;
    return result;
}
