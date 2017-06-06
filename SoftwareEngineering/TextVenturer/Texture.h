#pragma once

class Texture abstract
{
private:
    unsigned int ID;
    int unitID;

public:
    Texture();
    virtual ~Texture();

    void bind();

    void uniform(Shader & shader, std::string name) const;

    static void init();

    static bool initialized;
    static int maxUnits;
    static int unitCount;
    static Texture * boundTexture;
};

class SingleTexture : public Texture
{
private:
    TextureData data;

public:
    SingleTexture(std::string filename);
    const TextureData & getData() const;
};