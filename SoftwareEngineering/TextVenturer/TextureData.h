#pragma once

class TextureData
{
private:
    uvec2 size;
    byte* data;
    std::string name;

    TextureData() {};

public:
    TextureData(std::string filename);
    TextureData(uvec2 size, std::string name);
    TextureData(const TextureData & other);
    TextureData(TextureData && other);
    virtual ~TextureData();

    void freeData();

    uvec2 getSize() const;
    std::string getName() const;
    byte* getData() const;
};

