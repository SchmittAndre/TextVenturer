#include "stdafx.h"
#include "Registry.h"

#include "RegTools.h"

bool RegTools::installExtension(
    std::wstring extension, 
    std::wstring programName, 
    std::wstring description, 
    std::wstring openWith, 
    std::wstring iconPath, 
    int iconIndex, 
    std::wstring paramString)
{
    // can't define an empty extension
    if (extension.empty())
        return false;
    
    // add "." to extension if missing as in "abc" -> ".abc"
    if (extension[0] != L'.')
        extension = L"." + extension;

    // replace "*" in openWith with the full exe-path
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

    // replace "*" in iconPath with the exe-path
    if (iconPath == L"*")
        iconPath = openWith;

    using namespace Registry;

    // create the extension key
    if (Key ext = Key(HKCR, extension, Key::cmCreate))
    {
        if (StringValue* value = *ext.createDefaultValue(vtString))
            value->set(programName);
        else
            return false;
    }
    else
        return false;

    // createt the program key
    if (Key program = Key(HKCR, programName, Key::cmCreate))
    {
        // set the program key default key to the file description
        if (StringValue* value = *program.createDefaultValue(vtString))
            value->set(description);
        else
            return false;    

        // create the DefaultIcon key, containing the iconPath and iconIndex as its default value
        if (Key icon = Key(program, L"DefaultIcon", Key::cmCreate))
        {
            if (StringValue* value = *icon.createDefaultValue(vtString))
                value->set(iconPath + L"," + std::to_wstring(iconIndex));
            else
                return false;
        }
        else
            return false;
        
        // create Shell\Open\Command containing the full path to the exe as its default value
        if (Key shellOpenCmd = Key(program, L"Shell\\Open\\Command", Key::cmCreate))
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

    // notify windows, that an extension changed
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    return true;
}

bool RegTools::uninstallExtension(std::wstring extension)
{   
    // can't define an empty extension
    if (extension.empty())
        return false;

    // add "." to extension if missing as in "abc" -> ".abc"
    if (extension[0] != L'.')
        extension = L"." + extension;   

    using namespace Registry;         

    std::wstring programName;

    // key might not exist in the first place
    if (!Key(HKCR, extension))
        return true;

    // get the program key name out of the extension keys default value
    if (Key ext = Key(HKCR, extension, Key::cmReadOnly))
    {
        if (StringValue* value = *ext.getDefaultValue())
            programName = value->get();
        else
            return false;
    }
    else
        return false;

    // try to delete the extension key, now that we have the name
    if (!Key(HKCR).deleteKey(extension))
        return false;

    // test if the program key even exists
    if (!Key(HKCR, programName, Key::cmReadOnly))
        return true;
    
    // try to open it and delete all its subkeys, we created
    if (Key program = Key(HKCR, programName, Key::cmReadWrite))
    {
        // delete all subkeys
        if (!program.deleteKey(L"DefaultIcon"))
            return false;
        if (!program.deleteKey(L"Shell\\Open\\Command"))
            return false;
        if (!program.deleteKey(L"Shell\\Open"))
            return false;
        if (!program.deleteKey(L"Shell"))
            return false;
    }
    else
    {
        // don't have access or something other went wrong
        return false;
    }

    // try to delete the program key, this will fail, if anything is left in the key
    if (!Key(HKCR).deleteKey(programName))
        return false;

    // notify windows, that an extension changed
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    return true;
}

bool RegTools::extensionExists(std::wstring extension)
{                                             
    // can't define an empty extension
    if (extension.empty())
        return false;

    // add "." to extension if missing as in "abc" -> ".abc"
    if (extension[0] != L'.')
        extension = L"." + extension;

    // try to open the key
    return Registry::Key(Registry::HKCR, extension);
}
