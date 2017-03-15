#pragma once

class FBO
{
private:
    GLuint fbo;  

    std::unordered_map<GLFBOAttachment, BaseRBO*> outputs;

    UINT width;
    UINT height;

    static FBO* boundFBO;
public:
    FBO(UINT width, UINT height);
    virtual ~FBO();

    static void bindScreen(UINT width, UINT height);
    void bind();

    void enableRenderBuffer(GLFBOAttachment type, GLPixelFormat format);
    void enableRenderBufferMS(GLFBOAttachment type, GLPixelFormat format, UINT samples);

    bool finish();

    void copyTo(FBO* dest, GLAttribMask mask);
    void copyToScreen(GLAttribMask mask);

    void resize(UINT width, UINT height);
    void setSamples(UINT samples);
};

