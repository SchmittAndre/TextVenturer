#include "stdafx.h"

#include "FileStream.h"

FileStream::FileStream(const std::wstring & filename, std::ios::openmode mode)
    : std::fstream(filename, mode | std::ios::binary)
{
}
                  
// bool

void FileStream::write(bool value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), 1);
}

void FileStream::read(bool & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), 1);
}

bool FileStream::readBool()
{        
    bool value;
    read(value);
    return value;
}
        
// char
void FileStream::write(char value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(char));
}

void FileStream::read(char & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(char));
}

char FileStream::readChar()
{
    char value;
    read(value);
    return value;
}
          
// byte
void FileStream::write(byte value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(byte));
}

void FileStream::read(byte & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(byte));
}

byte FileStream::readByte()
{
    byte value;
    read(value);
    return value;
}

// short
void FileStream::write(short value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(short));
}

void FileStream::read(short & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(short));
}

short FileStream::readShort()
{
    short value;
    read(value);
    return value;
}        

// ushort
void FileStream::write(unsigned short value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(unsigned short));
}

void FileStream::read(unsigned short & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(unsigned short));
}

unsigned short FileStream::readUShort()
{
    unsigned short value;
    read(value);
    return value;
}       

// int
void FileStream::write(int value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(int));
}

void FileStream::read(int & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(int));
}

int FileStream::readInt()
{
    int value;
    read(value);
    return value;
}      

// uint
void FileStream::write(unsigned int value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(unsigned int));
}

void FileStream::read(unsigned int & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
}

unsigned int FileStream::readUInt()
{
    unsigned int value;
    read(value);
    return value;
}      

// int64

void FileStream::write(long long value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(long long));
}

void FileStream::read(long long & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(long long));
}

long long FileStream::readInt64()
{
    long long value;
    read(value);
    return value;
}
      
// uint64

void FileStream::write(unsigned long long value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(unsigned long long));
}

void FileStream::read(unsigned long long & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(unsigned long long));
}

unsigned long long FileStream::readUInt64()
{
    unsigned long long value;
    read(value);
    return value;
}

// float

void FileStream::write(float value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(float));
}

void FileStream::read(float & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(float));
}

float FileStream::readFloat()
{
    float value;
    read(value);
    return value;
}

// double

void FileStream::write(double value)
{
    std::fstream::write(reinterpret_cast<char*>(&value), sizeof(double));
}

void FileStream::read(double & value)
{
    std::fstream::read(reinterpret_cast<char*>(&value), sizeof(double));
}

double FileStream::readDouble()
{
    double value;
    read(value);
    return value;
}

// cstring

void FileStream::write(const char * text)
{
    unsigned int length = static_cast<unsigned int>(strlen(text));
    write(length);
    std::fstream::write(text, length);
}

void FileStream::read(char * text, std::streamsize count)
{
    std::fstream::read(text, count);
}
    
// string

void FileStream::write(const std::string & text)
{
    unsigned int length = static_cast<unsigned int>(text.size());
    write(length);
    std::fstream::write(text.c_str(), length);
}

void FileStream::read(std::string & text)
{
    unsigned int length;
    read(length);
    text.resize(length);
    std::fstream::read(const_cast<char*>(text.c_str()), length);
}

std::string FileStream::readString()
{
    std::string value;
    read(value);
    return value;
}

// taglist

void FileStream::write(const taglist & taglist)
{
    write(static_cast<UINT>(taglist.size()));
    for (std::string tag : taglist)
        write(tag);
}

void FileStream::read(taglist & taglist)
{
    UINT length;
    read(length);
    taglist.clear();
    taglist.reserve(length);
    for (UINT i = 0; i < length; i++)
        taglist.insert(readString());
}

taglist FileStream::readTags()
{
    taglist taglist;
    read(taglist);
    return taglist;
}

// stringlist

void FileStream::write(const stringlist & stringlist)
{
    write(static_cast<UINT>(stringlist.size()));
    for (std::string entry : stringlist)
        write(entry);
}

void FileStream::read(stringlist & stringlist)
{
    UINT length;
    read(length);
    stringlist.clear();
    stringlist.reserve(length);
    for (UINT i = 0; i < length; i++)
        stringlist.push_back(readString());
}

stringlist FileStream::readStrings()
{
    stringlist stringlist;
    read(stringlist);
    return stringlist;
}
