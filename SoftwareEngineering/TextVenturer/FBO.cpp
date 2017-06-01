#include "stdafx.h"

FBO * FBO::boundFBO = NULL;

FBO::FBO(UINT width, UINT height)
    : width(width)
    , height(height)
{
    glGenFramebuffers(1, &fbo);
}

FBO::~FBO()
{
    for (std::pair<GLFBOAttachment, BaseRBO*> output : outputs)
        delete output.second;

    glDeleteFramebuffers(1, &fbo);
}

void FBO::bindScreen(UINT width, UINT height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    boundFBO = NULL;
}

void FBO::bind()
{
    if (boundFBO != this)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, width, height);
        boundFBO = this;
    }
}

void FBO::enableRenderBuffer(GLFBOAttachment type, GLPixelFormat format)
{
    outputs[type] = new RBO(width, height, format);
    bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, outputs[type]->getID());
}

void FBO::enableRenderBufferMS(GLFBOAttachment type, GLPixelFormat format, UINT samples)
{
    outputs[type] = new RBOMS(width, height, format, samples);
    bind();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, outputs[type]->getID());
}

bool FBO::finish()
{
    bind();
    const GLenum drawbuffers = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, &drawbuffers);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    return status == GL_FRAMEBUFFER_COMPLETE;
}

void FBO::copyTo(FBO & dest, GLAttribMask mask)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest.fbo);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);

    boundFBO = NULL;
}

void FBO::copyToScreen(GLAttribMask mask)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, mask, GL_NEAREST);

    boundFBO = NULL;
}

void FBO::resize(UINT width, UINT height)
{
    this->width = width;
    this->height = height;
    for (std::pair<GLFBOAttachment, BaseRBO*> output : outputs)
    {
        if (RBO* rbo = dynamic_cast<RBO*>(output.second))
        {
            GLPixelFormat f = rbo->getFormat();
            delete rbo;
            enableRenderBuffer(output.first, f);
        }
        else if (RBOMS* rbo = dynamic_cast<RBOMS*>(output.second))
        {
            GLPixelFormat f = rbo->getFormat();
            UINT s = rbo->getSamples();
            delete rbo;
            enableRenderBufferMS(output.first, f, s);        
        }
    }
}

void FBO::setSamples(UINT samples)
{
    for (std::pair<GLFBOAttachment, BaseRBO*> output : outputs)
    {
        if (RBOMS* rbo = dynamic_cast<RBOMS*>(output.second))
        {
            GLPixelFormat f = rbo->getFormat();
            delete rbo;
            enableRenderBufferMS(output.first, f, samples);
        }
    }
}
