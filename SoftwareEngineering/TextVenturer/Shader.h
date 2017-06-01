#pragma once

class Shader
{
public:
    struct Attribute 
    {
        int count;
        std::string name;
        GLDataType type;

        Attribute(int count, std::string name, GLDataType type = dtFloat)
            : count(count)
            , name(name)
            , type(type)
        {
        }
    };

    typedef std::vector<Attribute> Attributes;

    class Location 
    {
    protected:
        typedef PFNGLGETUNIFORMLOCATIONPROC ShaderGetFunction;
    private:
        Shader & shader;
        std::string * name;
        int * value;
        virtual ShaderGetFunction getShaderGetFunction() = 0;
    public:
        Location(std::string name, Shader & shader);
        ~Location();
        int getValue();
        operator int();
    };

    friend Location;

    class UniformLocation : public Location 
    {
    protected:
        ShaderGetFunction getShaderGetFunction();
    public:
        UniformLocation(std::string name, Shader & shader);
    };
    
    class AttribLocation : public Location 
    {
    protected:
        ShaderGetFunction getShaderGetFunction();
    public:
        AttribLocation(std::string name, Shader & shader);
    };

private:
    int program;
    std::unordered_map<std::string, Location*> locations;

    Attributes attributes;

    static Shader * activeShader;

    bool linked;

    void checkShaderErrors(const std::wstring & shaderName, int shader) const;
    void checkProgramErrors() const;

public:
    Shader(Attributes attributes);
    virtual ~Shader();

    void addShaderFromFile(GLShaderType shaderType, const std::wstring & filename);
    void link();

    void loadVertFragShader(const std::wstring & filename);

    UniformLocation & getUniformLocation(const std::string & name);
    AttribLocation & getAttribLocation(const std::string & name);

    UINT getAttribCount() const;
    Attribute getAttribute(int i) const;

    bool hasAttributes() const;

    void enable();

    static void disable();
};

class VertexFragmentShader : public Shader
{
public:
    VertexFragmentShader(Attributes attributes, const std::wstring & filename);
};

class EShaderCompileError : public Exception
{
public:
    EShaderCompileError(const std::wstring & filename, const std::string & msg);
};

class EShaderNotLinked : public Exception
{
public:
    EShaderNotLinked();
};

class EShaderLocationWrongType : public Exception
{
public:
    EShaderLocationWrongType(const std::string & name);
};