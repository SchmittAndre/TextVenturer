#include "stdafx.h"

#include "StringUtils.h"

std::string strconv(std::wstring text)
{
    std::string ansi;
    ansi.reserve(text.length());
    for (auto c : text)
        ansi += static_cast<WORD>(c) <= 0xFF ? static_cast<char>(c) : '?';
    return ansi;
}

std::wstring strconv(std::string text)
{
    std::wstring wide;
    wide.reserve(text.length());
    for (auto c : text)
        wide += static_cast<wchar_t>(c);
    return wide;
}
