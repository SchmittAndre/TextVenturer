#include "stdafx.h"

VAO * VAO::boundVAO = 0;

Shader & VAO::getShader()
{
    return shader;
}

void VAO::beforeRender() const
{
    shader.enable();
}

void VAO::afterRender() const
{
    // don't do anything after rendering... can't be abstract coz it still has to be callable
}

VAO::VAO(Shader & shader, GLRenderMode renderMode)
    : shader(shader)
    , renderMode(renderMode)
    , stride(0)
    , pvbo(0)
{            
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(btArrayBuffer, vbo);
}

VAO::~VAO() 
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void VAO::bind()
{
    if (boundVAO != this)
    {
        glBindVertexArray(vao);
        boundVAO = this;
    }
}

void VAO::unbind() const
{
    glBindVertexArray(0);
    boundVAO = NULL;
}

void VAO::loadShaderAttributes()
{
    bind();
    for (UINT i = 0; i < shader.getAttribCount(); i++)
    {
        Shader::Attribute attrib = shader.getAttribute(i);
        int location = shader.getAttribLocation(attrib.name);
        void* offset = reinterpret_cast<void*>(stride);
        stride += attrib.count * getDataSize(attrib.type);

        if (location == -1)
            continue;

        glEnableVertexAttribArray(location);
        glVertexAttribPointer(
            location,
            attrib.count,
            attrib.type,
            blTrue,
            stride,
            offset);
    }

}

void VAO::generate(DWORD maxSize, GLBufferUsage usage)
{
    this->maxSize = maxSize;
    if (maxSize == 0)
        return;
    glBindBuffer(btArrayBuffer, vbo);
    glBufferData(btArrayBuffer, maxSize * stride, NULL, usage);
}

bool VAO::map(GLBufferAccess access)
{
    if (access == baWriteOnly)
        size = 0;
    if (maxSize == 0)
        return true;
    glBindBuffer(btArrayBuffer, vbo);
    pvbo = glMapBuffer(btArrayBuffer, access);
    if (!pvbo)
    {
        ErrorDialog("VAO not mappable!");
        return false;
    }
    vbopos = 0;
    return true;
}

void VAO::unmap()
{
    if (maxSize == 0)
        return;
    glUnmapBuffer(btArrayBuffer);
    pvbo = NULL;
}

bool VAO::addVertex(void * data)
{
    if (!pvbo)
    {
        ErrorDialog("VAO not mapped!");
        return false;
    }
    if (size >= maxSize)
    {
        ErrorDialog("VAO full!");
        return false;
    }
    memcpy((void*)((LONG_PTR)pvbo + vbopos), data, stride);
    vbopos += stride;
    size++;
    return true;
}

bool VAO::addVertices(DWORD count, void * data)
{
    if (!pvbo)
    {
        ErrorDialog("VAO not mapped!");
        return false;
    }
    if (size + count > maxSize)
    {
        ErrorDialog("VAO max size exceeded!");
        return false;
    }
    memcpy((void*)((LONG_PTR)pvbo + vbopos), data, stride * count);
    vbopos += stride * count;
    size += count;
    return true;
}

bool VAO::setVertex(DWORD offset, void * data) const
{
    if (offset >= maxSize)
    {
        ErrorDialog("VAO offset too high!");
        return false;
    }
    glBindBuffer(btArrayBuffer, vbo);
    glBufferSubData(btArrayBuffer, offset * stride, stride, data);
    return true;
}

bool VAO::setVertices(DWORD offset, DWORD count, void * data) const
{
    if (offset + count > maxSize)
    {
        ErrorDialog("VAO offset+count too high!");
        return false;
    }
    glBindBuffer(btArrayBuffer, vbo);
    glBufferSubData(btArrayBuffer, offset * stride, stride * count, data);
    return true;
}

void VAO::forceSize(DWORD size)
{
    if (size >= maxSize)
    {
        ErrorDialog("Forced VAO size must not be greater than maxSize");
    }
    this->size = size;
}

void VAO::forceMaxSize()
{
    size = maxSize;
}

void VAO::render()
{
    if (size == 0)
        return;

    if (!attributesInitialized)
        loadShaderAttributes();

    beforeRender();
    bind();
    glDrawArrays(renderMode, 0, size);
    afterRender();
}
