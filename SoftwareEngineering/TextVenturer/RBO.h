#pragma once

class BaseRBO
{
private:
    GLuint rbo;

    static BaseRBO* boundRBO;

public:
    BaseRBO();
    virtual ~BaseRBO();

    void bind();
    GLuint getID() const;
};

class RBO : public BaseRBO
{
private:
    GLPixelFormat format;
    size_t width;
    size_t height;

public:
    RBO(size_t width, size_t height, GLPixelFormat format);

    GLPixelFormat getFormat() const;
    size_t getWidth() const;
    size_t getHeight() const;
};

class RBOMS : public BaseRBO
{
private:
    GLPixelFormat format;
    size_t width;
    size_t height;
    size_t samples;

public:
    RBOMS(size_t width, size_t height, GLPixelFormat format, size_t samples);

    GLPixelFormat getFormat() const;
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getSamples() const;
};
