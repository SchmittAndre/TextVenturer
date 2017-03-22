#pragma once

class ErrorDialog
{
public:
    ErrorDialog(const std::string & title, const std::string & message);
    ErrorDialog(const std::wstring & title, const std::wstring & message);
    ErrorDialog(const std::string & message);
    ErrorDialog(const std::wstring & message);
};

