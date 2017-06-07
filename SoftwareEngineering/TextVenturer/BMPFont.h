#pragma once

class BMPFont
{
private:
    SingleTexture data;
    
    int size; // pixel size per char
    float widths[256];
    
    void generateWidths();

public:
    BMPFont(std::string filename);

    Color getPixel(byte c, int x, int y) const;
    float getWidth(byte c) const;
    void uniform(Shader & shader, std::string name) const;

    vec2 getTexCoord(byte c, vec2 texcoord) const;
    float getPixelWidth() const;
};

