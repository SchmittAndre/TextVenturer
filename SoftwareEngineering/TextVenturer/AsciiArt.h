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

    byte operator()(int x, int y);
    string operator()(int line);

    int getWidth();
    int getHeight();
};

