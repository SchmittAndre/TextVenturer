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

    lines.clear();

    if (!file.is_open())
    {
        lines.push_back("+-------------+");
        lines.push_back("|             |");
        lines.push_back("|   MISSING   |");
        lines.push_back("|             |");
        lines.push_back("+-------------+");
    }
    else
    {         
        char c;
        std::string line;
        while (!file.eof())
        {
            file.read(&c, 1);
            if (c == '\n')
            {
                lines.push_back(line);
                line = "";
            }
            else if (!file.eof())
                line += c;
        }

        lines.push_back(line);
        file.close();
    }

    height = (UINT)lines.size();
    width = 0;
    for (UINT i = 0; i < height; i++)
        width = max(width, (UINT)lines[i].size());
}

void AsciiArt::clear()
{
    lines.clear();
    width = 0;
    height = 0;
}


std::string AsciiArt::operator[](size_t line) const
{
    return lines[line];
}

UINT AsciiArt::getWidth() const
{
    return width;
}

UINT AsciiArt::getHeight() const
{
    return height;
}
