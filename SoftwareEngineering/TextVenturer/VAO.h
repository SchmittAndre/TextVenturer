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

    virtual void beforeRender();
    virtual void afterRender();
public:
	VAO(Shader* shader, GLRenderMode renderMode = rmTriangles);
	~VAO();

    void bind();
    void unbind();   
    
    void generate(DWORD maxSize, GLBufferUsage usage);

    bool map(GLBufferAccess access);
    void unmap();

    bool addVertex(void *data);
    bool setVertex(DWORD offset, void *data);
    bool setVertices(DWORD offset, DWORD count, void *data);

    void forceSize(DWORD size);
    void forceMaxSize();

    void render();
};

