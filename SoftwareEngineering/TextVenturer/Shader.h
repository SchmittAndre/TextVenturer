#pragma once

class Shader
{
public:
    struct Attribute {
        int count;
        string name;
        GLDataType type;

        Attribute(int count, string name, GLDataType type)
        {
            this->count = count;
            this->name = name;
            this->type = type;
        }
    };

private:
    int program;
    unordered_map<string, int*> locations;

    vector<Attribute> attributes;

    static Shader* activeShader;

    bool checkShaderErrors(string shaderName, int shader) const;
    bool checkProgramErrors() const;

public:
    Shader();
    virtual ~Shader();

    bool addShaderFromFile(GLShaderType shaderType, string filename);
    bool link();

    bool loadVertFragShader(string filename);

    int getUniformLocation(string name);
    int getAttribLocation(string name);

    void addAttribute(int count, string name, GLDataType type = dtFloat);

    int getAttribCount() const;
    Attribute getAttribute(int i) const;

    void enable();

    static void disable();
};

