#include "stdafx.h"

#include "ErrorHandling.h"

ErrorDialog::ErrorDialog(const std::string & title, const std::string & message)
{
    MessageBoxA(NULL, message.c_str(), title.c_str(), MB_ICONERROR);
}

ErrorDialog::ErrorDialog(const std::wstring & title, const std::wstring & message)
{
    MessageBoxW(NULL, message.c_str(), title.c_str(), MB_ICONERROR);
}

ErrorDialog::ErrorDialog(const std::string & message)
{
    ErrorDialog("Error", message);
}

ErrorDialog::ErrorDialog(const std::wstring & message)
{
    ErrorDialog(L"Error", message);
}

std::wstring getErrorWString(DWORD errorCode)
{
    LPWSTR buffer;
    DWORD size = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        0, errorCode, LANG_USER_DEFAULT, (LPWSTR)&buffer, 0, NULL);
    std::wstring result = buffer;
    LocalFree(buffer);
    return result;
}

std::string getErrorString(DWORD errorCode)
{
    LPSTR buffer;
    DWORD size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        0, errorCode, LANG_USER_DEFAULT, (LPSTR)&buffer, 0, NULL);
    std::string result = buffer;
    LocalFree(buffer);
    return result;
}

Exception::Exception(std::string msg)
{
    this->msg = msg;
    codepos = false;
}

Exception::~Exception()
{
}

char const * Exception::what() const
{
    return msg.c_str();
}

int Exception::getLine()
{
    return line;
}

std::string Exception::getFile()
{
    return file;
}

std::string Exception::getFunc()
{
    return func;
}

bool Exception::hasLineInfo() const
{
    return codepos;
}

std::string Exception::getLineInfo() const
{                                     
    return file + "(" + std::to_string(line) + ", 1)";
}

void Exception::debugOutput() const
{
    std::string text = getLineInfo() + ":\r\n  [Exception] " + what() + "\r\n";
    OutputDebugStringA(text.c_str());      
}

Exception & Exception::setInfo(int line, const char * func, const char * file)
{
    this->line = line;
    this->func = func;
    this->file = file;
    codepos = true;
    return *this;
}

ENotImplemented::ENotImplemented(std::string feature)
    : Exception("Feature not yet implemented: " + feature)
{
}
