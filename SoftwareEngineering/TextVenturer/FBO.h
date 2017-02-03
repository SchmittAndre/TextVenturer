#pragma once

class FBO
{
private:
    GLuint fbo;  

    std::unordered_map<GLFBOAttachment, BaseRBO*> outputs;

    size_t width;
    size_t height;

    static FBO* boundFBO;
public:
    FBO(size_t width, size_t height);
    virtual ~FBO();

    static void bindScreen(size_t width, size_t height);
    void bind();

    void enableRenderBuffer(GLFBOAttachment type, GLPixelFormat format);
    void enableRenderBufferMS(GLFBOAttachment type, GLPixelFormat format, size_t samples);

    bool finish();

    void copyTo(FBO* dest, GLAttribMask mask);
    void copyToScreen(GLAttribMask mask);

    void resize(size_t width, size_t height);
    void setSamples(size_t samples);
};

