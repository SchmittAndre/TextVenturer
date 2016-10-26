#include "stdafx.h"

Shader* Shader::activeShader = 0;

Shader::Shader()
{
    program = glCreateProgram();
}

Shader::~Shader()
{
    for (pair<string, int*> i : locations)
        delete i.second;
    glDeleteProgram(program);
}

bool Shader::checkShaderErrors(string shaderName, int shader)
{
    int blen;       
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen); 

    if (blen <= 1)
        return true;
    
    char* infoLog = new char[blen];   
    int slen;
    glGetShaderInfoLog(shader, blen, &slen, infoLog);
    ErrorDialog("Shader Error in " + shaderName, infoLog);     
    delete[] infoLog;

    return false;
}

bool Shader::checkProgramErrors()
{
    int blen;      
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &blen);  
    if (blen <= 1)
        return true;

    char* infoLog = new char[blen];
    int slen;
    glGetProgramInfoLog(program, blen, &slen, infoLog);
    ErrorDialog("Linking Error", infoLog);  
    delete[] infoLog;

    return false;
}

bool Shader::addShaderFromFile(GLShaderType shaderType, string filename)
{
    int shader = glCreateShader(shaderType);

    ifstream shaderFile(filename);
    if (!shaderFile.good())
    {
        ErrorDialog("Can't open shader file \"" + filename + "\"!");
        return false;
    }
    stringstream buffer;                 
    buffer << shaderFile.rdbuf();

    int length = buffer.str().size();
    char* data = new char[length + 1];
    buffer.read(data, length);
    data[length] = 0;

    glShaderSource(shader, 1, &data, &length);
    delete[] data;

    glCompileShader(shader);

    if (!checkShaderErrors(filename, shader))
        return false;

    glAttachShader(program, shader);
    glDeleteShader(shader);

    return true;
}

bool Shader::link()
{
    glLinkProgram(program);
    if (!checkProgramErrors())
        return false;
    enable();
    return true;
}

bool Shader::loadVertFragShader(string filename)
{
    string tmp = filename;
    return addShaderFromFile(stVertex, tmp.append(".vs")) &&
           addShaderFromFile(stFragment, filename.append(".fs")) &&
           link();
}

int Shader::getUniformLocation(string name)
{
    // save the location to minimize shader-gets
    int* l = locations[name];
    if (l)
        return *l;
    else
        return *(locations[name] = new int(glGetUniformLocation(program, name.c_str())));
}

int Shader::getAttribLocation(string name)
{
    // save the location to minimize shader-gets
    int* l = locations[name];
    if (l)
        return *l;
    else
        return *(locations[name] = new int(glGetAttribLocation(program, name.c_str())));
}

void Shader::addAttribute(int count, string name, GLDataType type)
{
    attributes.push_back(Attribute(count, name, type));
}

int Shader::getAttribCount()
{
    return attributes.size();
}

Shader::Attribute Shader::getAttribute(int i)
{
    return attributes[i];
}

void Shader::enable()
{
    if (activeShader != this)
    {
        glUseProgram(program);
        activeShader = this;
    }
}

void Shader::disable()
{
    glUseProgram(0);
}
