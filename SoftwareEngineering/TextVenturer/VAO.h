#pragma once

class VAO {
private:
    GLuint vao;
    GLuint vbo;

    void * pvbo;
    size_t vbopos;

    GLRenderMode renderMode;

    DWORD size;
    DWORD maxSize;
    GLsizei stride;

    Shader & shader;

    void loadShaderAttributes();

    static VAO * boundVAO;

protected:
    Shader & getShader();

    virtual void beforeRender() const;
    virtual void afterRender() const;

public:
	VAO(Shader & shader, GLRenderMode renderMode = rmTriangles);
	virtual ~VAO();

    void bind();
    void unbind() const;
    
    void generate(DWORD maxSize, GLBufferUsage usage);

    void map(GLBufferAccess access);
    void unmap();

    void addVertex(void * data);         
    void addVertices(DWORD count, void * data);

    void setVertex(DWORD offset, void * data);
    void setVertices(DWORD offset, DWORD count, void * data);

    void forceSize(DWORD size);
    void forceMaxSize();

    void render();
};

class EVAONotMappable : public Exception
{
public:
    EVAONotMappable();
};

class EVAONotMapped : public Exception
{
public:
    EVAONotMapped();
};

class EVAOTooSmall : public Exception
{
public:
    EVAOTooSmall(int requiredSize);
};
