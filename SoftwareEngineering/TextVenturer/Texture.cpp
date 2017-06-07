#include "stdafx.h"

bool Texture::initialized = false;
int Texture::unitCount = 0;
Texture * Texture::boundTexture = 0;
int Texture::maxUnits;

Texture::Texture()
{
    if (!initialized)
        init();

    glGenTextures(1, &ID);
    unitID = unitCount++;
    
    bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magNearest);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minNearest);
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
}

void Texture::bind()
{
    if (boundTexture != this)
    {
        glActiveTexture(GL_TEXTURE0 + unitID);
        glBindTexture(GL_TEXTURE_2D, ID);
        boundTexture = this;
    }
}

void Texture::uniform(Shader & shader, std::string name) const
{
    shader.enable();
    glUniform1i(shader.getUniformLocation(name), unitID);
}

void Texture::init()
{
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
    initialized = true;
}

SingleTexture::SingleTexture(std::string filename)
    : data(filename)
{
    bind();
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        data.getSize().x,
        data.getSize().y,
        0,
        GL_BGRA,
        GL_UNSIGNED_BYTE,
        data.getData()
    );
}

const TextureData & SingleTexture::getData() const
{
    return data;
}
