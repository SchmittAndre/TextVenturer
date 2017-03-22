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
    return buffer;
}

std::string getErrorString(DWORD errorCode)
{
    LPSTR buffer;
    DWORD size = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        0, errorCode, LANG_USER_DEFAULT, (LPSTR)&buffer, 0, NULL);
    return buffer;
}
