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

RBO::RBO(UINT width, UINT height, GLPixelFormat format)
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

UINT RBO::getWidth() const
{
    return width;
}

UINT RBO::getHeight() const
{
    return height;
}

RBOMS::RBOMS(UINT width, UINT height, GLPixelFormat format, UINT samples)
{
    this->format = format;
    this->width = width;
    this->height = height;
    this->samples = 1;
    bind();
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
}

GLPixelFormat RBOMS::getFormat() const
{
    return format;
}

UINT RBOMS::getWidth() const
{
    return width;
}

UINT RBOMS::getHeight() const
{
    return height;
}

UINT RBOMS::getSamples() const
{
    return samples;
}
