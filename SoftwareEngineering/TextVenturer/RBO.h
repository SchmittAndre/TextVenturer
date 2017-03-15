#pragma once

class BaseRBO
{
private:
    UINT rbo;

    static BaseRBO* boundRBO;

public:
    BaseRBO();
    virtual ~BaseRBO();

    void bind();
    UINT getID() const;
};

class RBO : public BaseRBO
{
private:
    GLPixelFormat format;
    UINT width;
    UINT height;

public:
    RBO(UINT width, UINT height, GLPixelFormat format);

    GLPixelFormat getFormat() const;
    UINT getWidth() const;
    UINT getHeight() const;
};

class RBOMS : public BaseRBO
{
private:
    GLPixelFormat format;
    UINT width;
    UINT height;
    UINT samples;

public:
    RBOMS(UINT width, UINT height, GLPixelFormat format, UINT samples);

    GLPixelFormat getFormat() const;
    UINT getWidth() const;
    UINT getHeight() const;
    UINT getSamples() const;
};
