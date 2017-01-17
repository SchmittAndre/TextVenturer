#include "stdafx.h"
#include "RBO.h"

BaseRBO* BaseRBO::boundRBO = NULL;

BaseRBO::BaseRBO()
{
    glGenRenderbuffers(1, &rbo);
}

BaseRBO::~BaseRBO()
{
    glDeleteRenderbuffers(1, &rbo);
}

void BaseRBO::bind()
{
    if (this != boundRBO)
    {
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        boundRBO = this;
    }
}

GLuint BaseRBO::getID() const
{
    return rbo;
}

RBO::RBO(size_t width, size_t height, GLPixelFormat format)
{
    this->format = format;
    this->width = width;
    this->height = height;
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

GLPixelFormat RBO::getFormat() const
{
    return format;
}

size_t RBO::getWidth() const
{
    return width;
}

size_t RBO::getHeight() const
{
    return height;
}

RBOMS::RBOMS(size_t width, size_t height, GLPixelFormat format, size_t samples)
{
    this->format = format;
    this->width = width;
    this->height = height;
    bind();
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
}

GLPixelFormat RBOMS::getFormat() const
{
    return format;
}

size_t RBOMS::getWidth() const
{
    return width;
}

size_t RBOMS::getHeight() const
{
    return height;
}

size_t RBOMS::getSamples() const
{
    return samples;
}
