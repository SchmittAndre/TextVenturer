#include "stdafx.h"

Shader* Shader::activeShader = NULL;

Shader::Shader()
{
    program = glCreateProgram();
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

bool Shader::checkShaderErrors(std::string shaderName, int shader) const
{
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status)
        return true;

    int blen;       
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen); 
    
    if (blen <= 1)
        ErrorDialog("Shader Error in " + shaderName, "Unknown error! :(");

    std::string infoLog;
    infoLog.resize(blen);

    int slen;
    glGetShaderInfoLog(shader, blen, &slen, (char*)infoLog.c_str());

    ErrorDialog("Shader Error in " + shaderName, infoLog);     

    return false;
}

bool Shader::checkProgramErrors() const
{
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status)
        return true;

    int blen;      
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &blen);  
   
    if (blen <= 1)
        ErrorDialog("Linking Error", "Unknown error! :(");

    std::string infoLog;
    infoLog.resize(blen);
    int slen;
    glGetProgramInfoLog(program, blen, &slen, (char*)infoLog.c_str());

    ErrorDialog("Linking Error", infoLog);  

    return false;
}

bool Shader::addShaderFromFile(GLShaderType shaderType, std::string filename)
{
    int shader = glCreateShader(shaderType);

    std::ifstream shaderFile(filename);
    if (!shaderFile.good())
    {
        ErrorDialog("Can't open shader file \"" + filename + "\"!");
        return false;
    }                          

    std::stringstream buffer;                 
    buffer << shaderFile.rdbuf();

    int length = (int)buffer.str().size();
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

bool Shader::loadVertFragShader(std::string filename)
{
    std::string tmp = filename;
    return addShaderFromFile(stVertex, tmp.append(".vs")) &&
           addShaderFromFile(stFragment, filename.append(".fs")) &&
           link();
}

int Shader::getUniformLocation(std::string name)
{
    // save the location to minimize shader-gets
    auto loc = locations.find(name);
    if (loc != locations.end())
        return loc->second;
    else
        return locations[name] = glGetUniformLocation(program, name.c_str());
}

int Shader::getAttribLocation(std::string name)
{
    // save the location to minimize shader-gets
    auto loc = locations.find(name);
    if (loc != locations.end())
        return loc->second;
    else
        return locations[name] = glGetAttribLocation(program, name.c_str());
}

void Shader::addAttribute(int count, std::string name, GLDataType type)
{
    attributes.push_back(Attribute(count, name, type));
}

UINT Shader::getAttribCount() const
{
    return (UINT)attributes.size();
}

Shader::Attribute Shader::getAttribute(int i) const
{
    return attributes[i];
}

bool Shader::hasAttributes() const
{
    return !attributes.empty();
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
    activeShader = NULL;
}
