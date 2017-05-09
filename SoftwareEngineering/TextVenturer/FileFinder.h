#pragma once

class FileFinder
{
private:
    std::wstring path;
    std::vector<WIN32_FIND_DATA> data;

public:
    FileFinder(std::wstring path);

    void update();
                                    
    std::vector<WIN32_FIND_DATA>::const_iterator begin();
    std::vector<WIN32_FIND_DATA>::const_iterator end();
};

std::wstring extractFileExtension(std::wstring filename);