#pragma once

class BMPFont
{
private:
    int size; // pixel size per char
    float widths[256];

    SingleTexture data;

    void generateWidths();

public:
    virtual ~BMPFont();

    void loadFromPNG(std::string filename);

    Color getPixel(byte c, int x, int y) const;
    float getWidth(byte c) const;
    void uniform(Shader* shader, std::string name) const;

    vec2 getTexCoord(byte c, vec2 texcoord) const;
    float getPixelWidth() const;
};

