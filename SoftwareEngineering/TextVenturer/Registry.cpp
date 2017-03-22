#include "stdafx.h"
#include "Registry.h"

HKEY Registry::Key::predefinedToHKEY(PredefinedKey key)
{
    switch (key)
    {
    case pkClassesRoot:
        return HKEY_CLASSES_ROOT;
    case pkCurrentConfig:
        return HKEY_CURRENT_CONFIG;
    case pkCurrentUser:
        return HKEY_CURRENT_USER;
    case pkCurrentUserLocalSettings:
        return HKEY_CURRENT_USER_LOCAL_SETTINGS;
    case pkLocalMachine:
        return HKEY_LOCAL_MACHINE;
    case pkPerformanceData:
        return HKEY_PERFORMANCE_DATA;
    case pkPerformanceNLSText:
        return HKEY_PERFORMANCE_NLSTEXT;
    case pkPerformanceText:
        return HKEY_PERFORMANCE_TEXT;
    case pkUsers:
        return HKEY_USERS;
    }
    return NULL;
}

void Registry::Key::constructor(const std::wstring & path, HKEY parent, bool canCreate)
{
    if (canCreate)
    {
        DWORD disposition;
        lastError = RegCreateKeyExW(parent, path.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &handle, &disposition);
        if (!lastError)
            switch (disposition)
            {
            case REG_CREATED_NEW_KEY:
                state = ksCreated;
                break;
            case REG_OPENED_EXISTING_KEY:
                state = ksOpened;
                break;
            default:
                state = ksError;
            }
    }
    else
    {
        lastError = RegOpenKeyExW(parent, path.c_str(), 0, KEY_ALL_ACCESS, &handle);
        if (!lastError)
            state = ksOpened;
    }
    if (lastError)
    {
        state = ksError;
        handle = NULL;
    }
}

Registry::Key::Key(const std::wstring & path, PredefinedKey parent, bool canCreate)
{
    constructor(path, predefinedToHKEY(parent), canCreate);
}

Registry::Key::Key(const std::wstring & path, Key & parent, bool canCreate)
{
    constructor(path, parent.handle, canCreate);
}
             
Registry::Key::~Key()
{
    while (!openValues.empty())
        delete openValues.back();
    RegCloseKey(handle);
}

bool Registry::Key::isOpen()
{
    return handle != NULL;
}

LSTATUS Registry::Key::getLastError()
{
    return lastError;
}

Registry::Key::State Registry::Key::getState()
{
    return state;
}

HKEY Registry::Key::getHKEY()
{
    return handle;
}

Registry::Value * Registry::Key::getValue(const std::wstring & name)
{
    ValueType type;
    DWORD size;
    lastError = RegGetValueW(handle, NULL, name.c_str(), RRF_RT_ANY, (LPDWORD)&type, NULL, &size);
    if (lastError)
        return NULL;
    return createValue(name, type);
}

Registry::Value * Registry::Key::createValue(const std::wstring & name, ValueType type)
{
    switch (type)
    {
    case vtString:
        return new StringValue(*this, name);
    }
    return NULL;
}

void Registry::Key::deleteValue(const std::wstring & name)
{
    RegDeleteValueW(handle, name.c_str());
}

Registry::Key::operator bool()
{
    return isOpen();
}

Registry::Key & Registry::Value::getKey()
{
    return key;
}

const std::wstring & Registry::Value::getName()
{
    return name;
}

Registry::Value::Value(Key & key, const std::wstring & name)
    : key(key)
{
    key.openValues.push_back(this);
    this->name = name;
}

Registry::Value::~Value()
{
    key.openValues.erase(std::find(key.openValues.cbegin(), key.openValues.cend(), this));
}

LSTATUS Registry::Value::getLastError()
{
    return lastError;
}

Registry::ValueType Registry::StringValue::getType()
{
    return vtString;
}

Registry::StringValue::StringValue(Key & key, const std::wstring & name)
    : Value(key, name)
{
}

std::wstring Registry::StringValue::get()
{
    std::wstring result;
    DWORD size;
    lastError = RegGetValueW(getKey().getHKEY(), NULL, getName().c_str(), RRF_RT_ANY, NULL, NULL, &size);
    if (lastError)
        return L"";
    result.resize(size / 2 - 2);
    lastError = RegGetValueW(getKey().getHKEY(), NULL, getName().c_str(), RRF_RT_ANY, NULL, (PVOID)result.c_str(), &size);
    if (lastError)
        return L"";
    return result;
}

bool Registry::StringValue::set(std::wstring text)
{
    lastError = RegSetValueExW(getKey().getHKEY(), getName().c_str(), 0, getType(), (BYTE*)text.c_str(), 2 * ((DWORD)text.size() + 1));
    return lastError == ERROR_SUCCESS;
}
