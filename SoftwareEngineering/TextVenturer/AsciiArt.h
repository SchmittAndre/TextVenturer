#pragma once

class AsciiArt
{
private:
    UINT width;
    UINT height;
    std::vector<std::string> lines;

public:
    AsciiArt();
    virtual ~AsciiArt();

    void loadFromFile(const std::string & filename);
    void clear();

    std::string operator[](size_t line) const;

    UINT getWidth() const;
    UINT getHeight() const;
};

