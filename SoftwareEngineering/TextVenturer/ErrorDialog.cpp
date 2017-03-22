#include "stdafx.h"

#include "ErrorDialog.h"

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
