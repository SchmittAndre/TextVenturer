#include "stdafx.h"

#include "FileFinder.h"

void FileFinder::update()
{
    WIN32_FIND_DATAW finddata;
    HANDLE findhandle = FindFirstFileW(path.c_str(), &finddata);
    if (findhandle == INVALID_HANDLE_VALUE)
        return; // TODO: throw exception here
    do
    {
        data.push_back(finddata);
    } while (FindNextFileW(findhandle, &finddata));
    FindClose(findhandle);
}

FileFinder::FileFinder(std::wstring path)
{
    this->path = path;
    update();
}

std::vector<WIN32_FIND_DATA>::const_iterator FileFinder::begin()
{
    return data.cbegin();
}

std::vector<WIN32_FIND_DATA>::const_iterator FileFinder::end()
{
    return data.cend();
}
