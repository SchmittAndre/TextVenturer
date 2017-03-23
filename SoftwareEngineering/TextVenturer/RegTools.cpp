#include "stdafx.h"
#include "Registry.h"

#include "RegTools.h"

bool RegTools::InstallExtension(
    std::wstring extension, 
    std::wstring programName, 
    std::wstring description, 
    std::wstring openWith, 
    std::wstring iconPath, 
    int iconIndex, 
    std::wstring paramString)
{
    if (extension.empty())
        return false;
    
    if (extension[0] != L'.')
        extension = L"." + extension;

    if (openWith == L"*")
    {
        wchar_t* buffer;
        DWORD bufferSize = 0x100;
        DWORD actualSize;
        while (true)
        {
            buffer = new wchar_t[bufferSize];
            actualSize = GetModuleFileNameW(NULL, buffer, bufferSize);
            if (actualSize == 0)
            {
                delete[] buffer;
                return false;
            }
            if (actualSize < bufferSize)
                break;
            delete[] buffer;
            bufferSize <<= 1;
        }           
        buffer[actualSize] = 0;
        openWith = buffer;
        delete[] buffer;
    }

    if (iconPath == L"*")
        iconPath = openWith;

    using namespace Registry;

    if (Key ext = Key(extension, pkClassesRoot, Key::cmCreate))
    {
        if (StringValue* value = *ext.createDefaultValue(vtString))
            value->set(programName);
        else
            return false;
    }
    else
        return false;

    if (Key program = Key(programName, pkClassesRoot, Key::cmCreate))
    {
        if (StringValue* value = *program.createDefaultValue(vtString))
            value->set(description);
        else
            return false;    

        if (Key icon = Key(L"DefaultIcon", program, Key::cmCreate))
        {
            if (StringValue* value = *icon.createDefaultValue(vtString))
                value->set(iconPath + L"," + std::to_wstring(iconIndex));
            else
                return false;
        }
        else
            return false;

        if (Key shellOpenCmd = Key(L"Shell\\Open\\Command", program, Key::cmCreate))
        {
            if (StringValue* value = *shellOpenCmd.createDefaultValue(vtString))
                value->set(L"\"" + openWith + L"\" \"" + paramString + L"\"");
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;                  

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    return true;
}

bool RegTools::UninstallExtension(std::wstring extension)
{              
    using namespace Registry;

    std::wstring programName;

    if (!Key(extension, pkClassesRoot))
        return true; // doesn't exist

    if (Key ext = Key(extension, pkClassesRoot, Key::cmCreate))
    {
        if (StringValue* value = *ext.getDefaultValue())
            programName = value->get();
    }
    else
    {
        return false;
    }

    Registry::Key(Registry::pkClassesRoot).deleteKey(extension);

    return true;
}
