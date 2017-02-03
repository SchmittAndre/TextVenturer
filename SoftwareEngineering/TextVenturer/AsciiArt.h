#pragma once

class AsciiArt
{
private:
    size_t width;
    size_t height;
    std::string* lines;

public:
    AsciiArt();
    virtual ~AsciiArt();

    void loadFromFile(const std::string & filename);
    void clear();

    std::string operator[](size_t line) const;

    size_t getWidth() const;
    size_t getHeight() const;
};

