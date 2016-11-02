#pragma once

class BMPFont
{
private:
    int size; // pixel size per char
    float widths[256];

    SingleTexture data;

    void generateWidths();

public:
    ~BMPFont();

    void loadFromPNG(string filename);

    Color getPixel(byte c, int x, int y);
    float getWidth(byte c);
    void uniform(Shader* shader, string name);

    vec2 getTexCoord(byte c, vec2 texcoord);
};

