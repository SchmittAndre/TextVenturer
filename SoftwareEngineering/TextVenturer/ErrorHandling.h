#pragma once

class Exception : public std::exception
{
private:
    std::string msg;
    bool codepos;

    int line;
    std::string file;
    std::string func;
    
public:
    Exception(std::string msg);
    virtual ~Exception();
    char const* what() const;

    int getLine();
    std::string getFile();
    std::string getFunc();

    bool hasLineInfo() const;
    std::string getLineInfo() const;

    void debugOutput() const;

    Exception& setInfo(int line, const char* func, const char* file);
};

#define throw(classname, ...) throw reinterpret_cast<classname&>(classname(__VA_ARGS__).setInfo(__LINE__, __FUNCSIG__, __FILE__))

class ENotImplemented : public Exception
{
public:
    ENotImplemented(std::string feature);
};

class ENotSupported : public Exception
{
public:
    ENotSupported(std::string feature);
};

class ErrorDialog
{
public:
    ErrorDialog(const std::string & title, const std::string & message);
    ErrorDialog(const std::wstring & title, const std::wstring & message);
    ErrorDialog(const std::string & message);
    ErrorDialog(const std::wstring & message);
};

std::wstring getErrorWString(DWORD errorCode);
std::string getErrorString(DWORD errorCode);

void throwGLError();

// use release to eliminate any leftover debugGLError
#ifdef _DEBUG
#define debugGLError() try { throwGLError(); } catch (Exception) { DebugBreak(); }
#endif