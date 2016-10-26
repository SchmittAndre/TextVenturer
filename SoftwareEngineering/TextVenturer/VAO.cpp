#include "stdafx.h"

VAO * VAO::boundVAO = 0;

Shader * VAO::getShader()
{
    return shader;
}

void VAO::beforeRender()
{
    shader->enable();
}

void VAO::afterRender()
{
    // don't do anything after rendering... can't be abstract coz it still has to be callable
}

VAO::VAO(Shader* shader, GLRenderMode renderMode)
{
    this->shader = shader;
    this->renderMode = renderMode;

    location = shader->getUniformLocation(name);

    stride = 0;
    pvbo = 0;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(btArrayBuffer, vbo);

    for (int i = 0; i < shader->getAttribCount(); i++)
    {
        Shader::Attribute a = shader->getAttribute(i);
        addAttribute(a.count, a.name, a.type);
    }
    genAttributes();
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

void VAO::unbind()
{
    glBindVertexArray(0);
    boundVAO = NULL;
}

void VAO::addAttribute(DWORD count, string name, GLDataType dataType)
{
    Attribute attrib;
    attrib.location = shader->getAttribLocation(name);
    attrib.dataCount = count;
    attrib.dataType = dataType;
    attrib.dataSize = count * getDataSize(dataType);
    attrib.offset = stride;
    stride += attrib.dataSize;
    attributes.push_back(attrib);
}

void VAO::genAttributes()
{
    bind();
    for (Attribute attrib : attributes)
    {
        if (attrib.location == -1)
            continue;
        glEnableVertexAttribArray(attrib.location);
        glVertexAttribPointer(
            attrib.location, 
            attrib.dataCount, 
            attrib.dataType, 
            blTrue, 
            stride, 
            (void*)attrib.offset);
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
    glBindBuffer(btArrayBuffer, 0);
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
    memcpy((void*)((DWORD)pvbo + vbopos), data, stride);
    vbopos += stride;
    size++;
    return true;
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
