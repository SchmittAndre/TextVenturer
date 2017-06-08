#pragma once

class FileStream : public std::fstream
{
private:
    std::streampos length;

public:
    FileStream(const std::wstring & filename, std::ios::openmode mode);

    void safeRead(char * data, std::streamsize count);

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
    void read(char* text, std::streamsize count);
    
    // string
    void write(const std::string & text);
    void read(std::string & text);
    std::string readString();

    // taglist
    void write(const taglist & taglist);
    void read(taglist & taglist);
    taglist readTags();

    // stringlist
    void write(const stringlist & stringlist);
    void read(stringlist & stringlist);
    stringlist readStrings();
};

class EFileOpenError : public Exception
{
public:
    EFileOpenError(std::wstring filename);
};

class EBinaryDamaged : public Exception
{
public:
    EBinaryDamaged();
};