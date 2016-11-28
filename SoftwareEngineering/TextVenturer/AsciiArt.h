#pragma once

class AsciiArt
{
private:
    int width;
    int height;
    string* lines;

public:
    AsciiArt();
    ~AsciiArt();

    void loadFromFile(string filename);
    void clear();

    string operator[](int line) const;

    int getWidth() const;
    int getHeight() const;
};

