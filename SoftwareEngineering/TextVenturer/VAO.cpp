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
    , size(0)
    , maxSize(0)
{            
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(btArrayBuffer, vbo);    
    loadShaderAttributes();
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
        stride += attrib.count * getDataSize(attrib.type);
    }

    UINT_PTR offset = 0;
    for (UINT i = 0; i < shader.getAttribCount(); i++)
    {
        Shader::Attribute attrib = shader.getAttribute(i);
        int location = shader.getAttribLocation(attrib.name);
        
        if (location != -1)
        {

            glEnableVertexAttribArray(location);
            glVertexAttribPointer(
                location,
                attrib.count,
                attrib.type,
                blTrue,
                stride,
                reinterpret_cast<void*>(offset));
        }      

        offset += attrib.count * getDataSize(attrib.type);
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

void VAO::map(GLBufferAccess access)
{
    if (access == baWriteOnly)
        size = 0;
    if (maxSize == 0)
        return;
    glBindBuffer(btArrayBuffer, vbo);
    pvbo = glMapBuffer(btArrayBuffer, access);
    if (!pvbo)
        throw(EVAONotMappable);
    vbopos = 0;
}

void VAO::unmap()
{
    if (maxSize == 0)
        return;
    glUnmapBuffer(btArrayBuffer);
    pvbo = NULL;
}

void VAO::addVertex(void * data)
{
    if (!pvbo)
        throw(EVAONotMapped);
    if (size >= maxSize)
        throw(EVAOTooSmall, 1);
    memcpy((void*)((LONG_PTR)pvbo + vbopos), data, stride);
    vbopos += stride;
    size++;
}

void VAO::addVertices(DWORD count, void * data)
{
    if (!pvbo)
        throw(EVAONotMapped);
    if (size + count > maxSize)
        throw(EVAOTooSmall, size + count - maxSize);
    memcpy((void*)((LONG_PTR)pvbo + vbopos), data, stride * count);
    vbopos += stride * count;
    size += count;
}

void VAO::setVertex(DWORD offset, void * data) 
{
    if (offset >= maxSize)
        throw(EVAOTooSmall, offset - maxSize);
    glBindBuffer(btArrayBuffer, vbo);
    glBufferSubData(btArrayBuffer, offset * stride, stride, data);
}

void VAO::setVertices(DWORD offset, DWORD count, void * data) 
{
    if (offset + count > maxSize)
        throw(EVAOTooSmall, offset + count - maxSize);
    glBindBuffer(btArrayBuffer, vbo);
    glBufferSubData(btArrayBuffer, offset * stride, stride * count, data);
}

void VAO::forceSize(DWORD size)
{
    if (size >= maxSize)
        throw(EVAOTooSmall, size - maxSize);
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

    beforeRender();
    bind();
    glDrawArrays(renderMode, 0, size);
    afterRender();
}

EVAONotMappable::EVAONotMappable()
    : Exception("VAO is not mappable")
{
}

EVAOTooSmall::EVAOTooSmall(int requiredSize)
    : Exception("VAO is " + std::to_string(requiredSize) + " units too small")
{
}

EVAONotMapped::EVAONotMapped()
    : Exception("VAO is not mapped")
{
}
