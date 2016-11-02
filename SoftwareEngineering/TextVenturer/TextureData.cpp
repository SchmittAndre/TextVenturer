#include "stdafx.h"

TextureData::TextureData(string filename)
{
    vector<byte> filedata;
    lodepng::decode(filedata, size.x, size.y, filename);
    data = new byte[filedata.size()];
    memcpy(data, filedata.data(), filedata.size());

    char *s = (char*)filedata.data();
    char *d = (char*)data;
    for (unsigned int y = 0; y < size.y; y++)
        memcpy(d + y * size.x * 4, s + (size.y - y - 1) * size.x * 4, size.x * 4);

    // extract C:\PATH/[filename.ext]                            
    size_t a = filename.rfind('/');
    size_t b = filename.rfind('\\');
    size_t pos;
    if (a == string::npos)
    {
        if (b == string::npos)
            pos = string::npos;
        else
            pos = b;
    }
    else
    {
        if (b == string::npos)
            pos = a;
        else
            pos = max(a, b);
    }

    if (pos == string::npos)             
        name = filename;
    else
        name = filename.substr(pos + 1); 

    // extract [filename].ext
    pos = name.find('.');
    if (pos != string::npos)
        name = name.substr(0, pos);
}

TextureData::TextureData(cvec2 size, string name)
{
    data = new byte[size.x * size.y * 4];
    this->name = name;
}

TextureData::TextureData(TextureData & other)
{
    name = other.name;
    size = other.size;
    int bytesize = size.x * size.y * 4;
    data = new byte[bytesize];
    memcpy(data, other.data, bytesize);
}

TextureData::~TextureData()
{
    freeData();
}

void TextureData::freeData()
{
    if (!data)
        return;
    delete[] data;
    data = NULL;
}

cvec2 TextureData::getSize()
{
    return size;
}

string TextureData::getName()
{
    return name;
}

byte * TextureData::getData()
{
    return data;
}
