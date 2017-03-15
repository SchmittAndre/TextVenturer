#pragma once

class Shader
{
public:
    struct Attribute {
        int count;
        std::string name;
        GLDataType type;

        Attribute(int count, std::string name, GLDataType type)
        {
            this->count = count;
            this->name = name;
            this->type = type;
        }
    };

private:
    int program;
    std::unordered_map<std::string, int*> locations;

    std::vector<Attribute> attributes;

    static Shader* activeShader;

    bool checkShaderErrors(std::string shaderName, int shader) const;
    bool checkProgramErrors() const;

public:
    Shader();
    virtual ~Shader();

    bool addShaderFromFile(GLShaderType shaderType, std::string filename);
    bool link();

    bool loadVertFragShader(std::string filename);

    int getUniformLocation(std::string name);
    int getAttribLocation(std::string name);

    void addAttribute(int count, std::string name, GLDataType type = dtFloat);

    UINT getAttribCount() const;
    Attribute getAttribute(int i) const;

    void enable();

    static void disable();
};

