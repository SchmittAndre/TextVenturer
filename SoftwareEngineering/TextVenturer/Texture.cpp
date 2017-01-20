#include "stdafx.h"

bool Texture::initialized = false;
int Texture::unitCount = 0;
Texture* Texture::boundTexture = 0;
int Texture::maxUnits;

Texture::Texture()
{
    if (!initialized)
        init();

    glGenTextures(1, &ID);
    unitID = unitCount++;
    
    bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magLinear);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minLinear);
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

void Texture::uniform(Shader * shader, string name) const
{
    shader->enable();
    glUniform1i(shader->getUniformLocation(name), unitID);
}

void Texture::init()
{
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
    initialized = true;
}

SingleTexture::SingleTexture()
{
    referenced = true;
}

SingleTexture::SingleTexture(string filename)
{
    referenced = false;
    texture = new TextureData(filename);
}

TextureData * SingleTexture::getTexture() const
{
    return texture;
}

void SingleTexture::setTexture(TextureData * texture)
{
    if (this->texture == texture)
        return;
    if (this->texture && !referenced) 
        delete this->texture;
    this->texture = texture;
    bind();
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        texture->getSize().x,
        texture->getSize().y,
        0,
        GL_BGRA,
        GL_UNSIGNED_BYTE,
        this->texture->getData()
        );
    referenced = true;
}
