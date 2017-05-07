#pragma once

class VAO {
private:
    GLuint vao;
    GLuint vbo;

    void* pvbo;
    size_t vbopos;

    GLRenderMode renderMode;

    DWORD size;
    DWORD maxSize;
    UINT_PTR stride;

    bool attributesInitialized;

    Shader & shader;

    void loadShaderAttributes();

    static VAO* boundVAO;

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

    bool map(GLBufferAccess access);
    void unmap();

    bool addVertex(void * data);         
    bool addVertices(DWORD count, void * data);

    bool setVertex(DWORD offset, void * data) const;
    bool setVertices(DWORD offset, DWORD count, void * data) const;

    void forceSize(DWORD size);
    void forceMaxSize();

    void render();
};

