#pragma once

namespace RegTools
{
    bool InstallExtension(
        std::wstring extension,
        std::wstring programName,
        std::wstring description,
        std::wstring openWith = L"*",
        std::wstring iconPath = L"*", 
        int iconIndex = 0,
        std::wstring paramString = L"%1");

    bool UninstallExtension(std::wstring extension);
}