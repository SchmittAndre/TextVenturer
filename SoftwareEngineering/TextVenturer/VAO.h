#pragma once

class VAO {
private:
    struct Attribute
    {
        int location;
        DWORD dataCount;
        GLDataType dataType;
        DWORD dataSize;
        DWORD offset;
    };

    GLuint vao;
    GLuint vbo;

    void* pvbo;
    DWORD vbopos;

    GLRenderMode renderMode;

    DWORD size;
    DWORD maxSize;
    DWORD stride;

    vector<Attribute> attributes;
    Shader* shader;

    void addAttribute(DWORD count, string name, GLDataType dataType = dtFloat);
    void genAttributes();

    static VAO* boundVAO;

protected:
    Shader* getShader();

    virtual void beforeRender() const;
    virtual void afterRender() const;
public:
	VAO(Shader* shader, GLRenderMode renderMode = rmTriangles);
	virtual ~VAO();

    void bind();
    void unbind() const;
    
    void generate(DWORD maxSize, GLBufferUsage usage);

    bool map(GLBufferAccess access);
    void unmap();

    bool addVertex(void *data);
    bool setVertex(DWORD offset, void *data) const;
    bool setVertices(DWORD offset, DWORD count, void *data) const;

    void forceSize(DWORD size);
    void forceMaxSize();

    void render();
};

