#pragma once

class TextureData
{
private:
    uvec2 size;
    byte* data;
    string name;

    TextureData() {};

public:
    TextureData(string filename);
    TextureData(uvec2 size, string name);
    TextureData(TextureData &other);
    virtual ~TextureData();

    void freeData();

    uvec2 getSize() const;
    string getName() const;
    byte* getData() const;
};

