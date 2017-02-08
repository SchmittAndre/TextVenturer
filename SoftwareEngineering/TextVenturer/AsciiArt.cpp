#include "stdafx.h"

#include "AsciiArt.h"

AsciiArt::AsciiArt()
{
    clear();
}

AsciiArt::~AsciiArt()
{
    clear();
}

void AsciiArt::loadFromFile(const std::string & filename)
{
    std::ifstream file(filename);

    std::vector<std::string> data;
    char c;
    std::string line;
    while (!file.eof())
    {
        file.read(&c, 1);
        if (c == '\n')
        {
            data.push_back(line);
            line = "";
        }
        else if (!file.eof())
            line += c;
    }
    data.push_back(line);
    file.close();
    
    clear();

    height = data.size();
    for (size_t i = 0; i < height; i++)
        width = max(width, (int)data[i].length());
    
    lines = new std::string[height];
    for (size_t i = 0; i < height; i++)
    {
        lines[i] = data[i]; 
        for (size_t x = data[i].length(); x < width; x++)
            lines[i].append(" ");
    }
}

void AsciiArt::clear()
{
    if (lines)
    {
        delete[] lines;
        lines = NULL;
    }
    width = 0;
    height = 0;
}


std::string AsciiArt::operator[](size_t line) const
{
    return lines[line];
}

size_t AsciiArt::getWidth() const
{
    return width;
}

size_t AsciiArt::getHeight() const
{
    return height;
}
