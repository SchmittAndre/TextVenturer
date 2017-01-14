#pragma once

class TextureData
{
private:
    cvec2 size;
    byte* data;
    string name;

    TextureData() {};

public:
    TextureData(string filename);
    TextureData(cvec2 size, string name);
    TextureData(TextureData &other);
    virtual ~TextureData();

    void freeData();

    cvec2 getSize() const;
    string getName() const;
    byte* getData() const;
};

