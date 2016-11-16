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

void AsciiArt::loadFromFile(string filename)
{
    ifstream file(filename);

    vector<string> data;
    char c;
    string line;
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
    for (int i = 0; i < height; i++)
        width = max(width, (int)data[i].length());
    
    lines = new string[height];
    for (int i = 0; i < height; i++)
    {
        lines[i] = data[i]; 
        for (int x = data[i].length(); x < width; x++)
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

byte AsciiArt::operator()(int x, int y)
{
    return byte(lines[y][x]);
}

string AsciiArt::operator()(int line)
{
    return lines[line];
}

int AsciiArt::getWidth()
{
    return width;
}

int AsciiArt::getHeight()
{
    return height;
}
