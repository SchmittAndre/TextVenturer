#include "stdafx.h"

Shader* Shader::activeShader = NULL;

Shader::Shader(Attributes attributes)
    : program(glCreateProgram())
    , linked(false)
    , attributes(attributes)
{
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::checkShaderErrors(const std::wstring & shaderName, int shader) const
{
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status)
        return;

    int blen;       
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen); 
    
    if (blen <= 1)
        throw(EShaderCompileError, shaderName, "Unknown error");

    std::string infoLog;
    infoLog.resize(blen);

    int slen;
    glGetShaderInfoLog(shader, blen, &slen, (char*)infoLog.c_str());

    throw(EShaderCompileError, shaderName, infoLog);     
}

void Shader::checkProgramErrors() const
{
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status)
        return;

    int blen;      
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &blen);  
   
    if (blen <= 1)
        throw(EShaderCompileError, L"Linking", "Unknown error! :(");

    std::string infoLog;
    infoLog.resize(blen);
    int slen;
    glGetProgramInfoLog(program, blen, &slen, (char*)infoLog.c_str());

    throw(EShaderCompileError, L"Linking", infoLog);
}

void Shader::addShaderFromFile(GLShaderType shaderType, const std::wstring & filename)
{
    int shader = glCreateShader(shaderType);

    std::ifstream shaderFile(filename);
    if (!shaderFile.good())
        throw(EFileOpenError, filename);

    std::stringstream buffer;                 
    buffer << shaderFile.rdbuf();

    int length = (int)buffer.str().size();
    char* data = new char[length + 1];
    buffer.read(data, length);
    data[length] = 0;

    glShaderSource(shader, 1, &data, &length);
    delete[] data;

    glCompileShader(shader);

    checkShaderErrors(filename, shader);

    glAttachShader(program, shader);
    glDeleteShader(shader);
}

void Shader::link()
{
    glLinkProgram(program);
    checkProgramErrors();
    linked = true;
    enable();
}

void Shader::loadVertFragShader(const std::wstring & filename)
{
    addShaderFromFile(stVertex, filename + L".vs");
    addShaderFromFile(stFragment, filename + L".fs");
    link();
}

Shader::UniformLocation & Shader::getUniformLocation(const std::string & name)
{
    // save the location to minimize shader-gets
    if (locations[name])
    {
        try
        {
            return dynamic_cast<UniformLocation&>(*locations[name]);
        }
        catch (std::bad_cast)
        {
            throw(EShaderLocationWrongType, name);
        }
    }
    else
    {
        UniformLocation * result = new UniformLocation(name, *this);
        locations[name] = result;
        return *result;
    }
}

Shader::AttribLocation & Shader::getAttribLocation(const std::string & name)
{
    // save the location to minimize shader-gets
    if (locations[name])
    {
        try
        {
            return dynamic_cast<AttribLocation&>(*locations[name]);
        }
        catch (std::bad_cast)
        {
            throw(EShaderLocationWrongType, name);
        }
    }
    else
    {
        AttribLocation * result = new AttribLocation(name, *this);
        locations[name] = result;
        return *result;
    }
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

Shader::Location::Location(std::string name, Shader & shader)
    : shader(shader)
    , value(NULL)
    , name(new std::string(name))
{                     
}

Shader::Location::~Location()
{
    delete value;                
    delete name;
}

int Shader::Location::getValue()
{
    if (!value)
    {
        if (!shader.linked)
            throw(EShaderNotLinked);
        value = new int(getShaderGetFunction()(shader.program, name->c_str()));
        delete name;
        name = NULL;
    }
    return *value;
}

Shader::Location::operator int()
{
    return getValue();
}

Shader::UniformLocation::UniformLocation(std::string name, Shader & shader)
    : Location(name, shader)
{
}

Shader::Location::ShaderGetFunction Shader::UniformLocation::getShaderGetFunction()
{
    return glGetUniformLocation;
}

Shader::AttribLocation::AttribLocation(std::string name, Shader & shader)
    : Location(name, shader) 
{
}

Shader::Location::ShaderGetFunction Shader::AttribLocation::getShaderGetFunction()
{
    return glGetAttribLocation;
}

EShaderLocationWrongType::EShaderLocationWrongType(const std::string & name)
    : Exception("Shader location \"" + name + "\" was previously defined with a different type")
{
}

EShaderCompileError::EShaderCompileError(const std::wstring & filename, const std::string & msg)
    : Exception("Shader Error: " + strconv(filename) + "\r\n" + msg)
{
}

EShaderNotLinked::EShaderNotLinked()
    : Exception("Shader not linked")
{
}

VertexFragmentShader::VertexFragmentShader(Attributes attributes, const std::wstring & filename)
    : Shader(attributes)
{
    loadVertFragShader(filename);
}
