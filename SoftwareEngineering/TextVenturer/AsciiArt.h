#pragma once

class AsciiArt
{
private:
    size_t width;
    size_t height;
    string* lines;

public:
    AsciiArt();
    virtual ~AsciiArt();

    void loadFromFile(const string & filename);
    void clear();

    string operator[](size_t line) const;

    size_t getWidth() const;
    size_t getHeight() const;
};

