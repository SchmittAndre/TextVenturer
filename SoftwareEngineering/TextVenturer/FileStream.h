#pragma once

class FileStream : public std::fstream
{
public:
    FileStream(const std::string & filename, std::ios::openmode mode);

    // bool
    void write(bool value);
    void read(bool & value);
    bool readBool();

    // char
    void write(char value);
    void read(char & value);
    char readChar();

    // byte
    void write(byte value);
    void read(byte & value);
    byte readByte();

    // short
    void write(short value);
    void read(short & value);
    short readShort();

    // ushort
    void write(unsigned short value);
    void read(unsigned short & value);
    unsigned short readUShort();

    // int
    void write(int value);
    void read(int & value);
    int readInt();

    // uint
    void write(unsigned int value);
    void read(unsigned int & value);
    unsigned int readUInt();

    // int64
    void write(long long value);
    void read(long long & value);
    long long readInt64();

    // uint64
    void write(unsigned long long value);
    void read(unsigned long long & value);
    unsigned long long readUInt64();

    // float
    void write(float value);
    void read(float & value);
    float readFloat();

    // double
    void write(double value);
    void read(double & value);
    double readDouble();

    // cstring
    void write(const char* text);
    
    // string
    void write(const std::string & text);
    void read(std::string & text);
    std::string readString();

    // tags
    void write(const tags & taglist);
    void read(tags & taglist);
    tags readTags();

    // strings
    void write(const strings & stringlist);
    void read(strings & stringlist);
    strings readStrings();
};