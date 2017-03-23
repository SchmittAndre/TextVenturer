#include "stdafx.h"
#include "Registry.h"

void Registry::Key::constructor(const std::wstring & path, HKEY parent, CreationMode mode)
{
    switch (mode)
    {
    case cmReadOnly:
        lastError = RegOpenKeyExW(parent, path.c_str(), 0, KEY_READ, &handle);
        if (!lastError)
            state = ksOpened;
        break;
    case cmReadWrite:
        lastError = RegOpenKeyExW(parent, path.c_str(), 0, KEY_READ | KEY_WRITE, &handle);
        if (!lastError)
            state = ksOpened;
        break;
    case cmCreate:
        DWORD disposition;
        lastError = RegCreateKeyExW(parent, path.c_str(), 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &handle, &disposition);
        if (!lastError)
        {
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
        break;
    default:
        lastError = ERROR_INVALID_PARAMETER;
    }
    if (lastError)
    {
        if (lastError == ERROR_FILE_NOT_FOUND)
            state = ksMissing;
        state = ksError;
        handle = NULL;
    }
}

Registry::Key::Key(PredefinedKey parent)
{
    handle = reinterpret_cast<HKEY>(parent);
    state = ksOpened;
    lastError = ERROR_SUCCESS;
}

Registry::Key::Key(const std::wstring & path, PredefinedKey parent, CreationMode mode)
{
    constructor(path, reinterpret_cast<HKEY>(parent), mode);
}

Registry::Key::Key(const std::wstring & path, Key & parent, CreationMode mode)
{
    constructor(path, parent.handle, mode);
}
             
Registry::Key::~Key()
{
    while (!openValues.empty())
        delete openValues.back();
    if (handle)
        RegCloseKey(handle);
}

bool Registry::Key::exists()
{
    return state != ksMissing;
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
    case vtMultiString:
        return new MultiStringValue(*this, name);
    case vtDWORD:
        return new DWORDValue(*this, name);
    case vtQWORD:
        return new QWORDValue(*this, name);
    }
    return NULL;
}

void Registry::Key::deleteValue(const std::wstring & name)
{
    RegDeleteValueW(handle, name.c_str());
}

Registry::Value * Registry::Key::getDefaultValue()
{
    return getValue(L"");
}

Registry::Value * Registry::Key::createDefaultValue(ValueType type)
{
    return createValue(L"", type);
}

void Registry::Key::deleteDefaultValue()
{
    deleteValue(L"");
}

bool Registry::Key::deleteKey(const std::wstring & name)
{
    lastError = RegDeleteKeyW(handle, name.c_str());
    return lastError == ERROR_SUCCESS;
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

Registry::Value::operator Registry::StringValue*()
{
    return dynamic_cast<StringValue*>(this);
}

Registry::Value::operator Registry::MultiStringValue*()
{
    return dynamic_cast<MultiStringValue*>(this);
}

Registry::Value::operator Registry::DWORDValue*()
{
    return dynamic_cast<DWORDValue*>(this);
}

Registry::Value::operator Registry::QWORDValue*()
{
    return dynamic_cast<QWORDValue*>(this);
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

Registry::MultiStringValue::MultiStringValue(Key & key, const std::wstring & name) 
    : Value(key, name)
{
}

std::vector<std::wstring> Registry::MultiStringValue::get()
{
    std::vector<std::wstring> result;
    
    DWORD size;
    lastError = RegGetValueW(getKey().getHKEY(), NULL, getName().c_str(), RRF_RT_ANY, NULL, NULL, &size);
    if (lastError)
        return result;

    wchar_t* data = new wchar_t[size];
    lastError = RegGetValueW(getKey().getHKEY(), NULL, getName().c_str(), RRF_RT_ANY, NULL, data, &size);
    if (lastError)
        return result;

    while (size_t len = wcslen(data))
    {
        result.push_back(data);
        data += len + 1;
    }

    delete[] data;

    return result;
}

bool Registry::MultiStringValue::set(std::vector<std::wstring> text)
{
    DWORD size = 1; 
    for (std::wstring str : text)
        size += (DWORD)str.size() + 1;
    wchar_t* data = new wchar_t[size];
    wchar_t* pos = data;
    for (std::wstring str : text)
    {
        StrCpyW(pos, str.c_str());
        pos += str.size();
        *pos = 0;
        pos++;
    }
    *pos = 0;
    lastError = RegSetValueExW(getKey().getHKEY(), getName().c_str(), 0, getType(), (BYTE*)data, size * 2);
    delete[] data;
    return lastError == ERROR_SUCCESS;
}

Registry::ValueType Registry::MultiStringValue::getType()
{
    return vtMultiString;
}

Registry::DWORDValue::DWORDValue(Key & key, const std::wstring & name)
    : Value(key, name)
{
}

UINT32 Registry::DWORDValue::get()
{
    UINT32 result;
    DWORD size = sizeof(result);
    lastError = RegGetValueW(getKey().getHKEY(), NULL, getName().c_str(), RRF_RT_ANY, NULL, &result, &size);
    if (lastError)
        return 0;
    return result;
}

bool Registry::DWORDValue::set(UINT32 value)
{
    lastError = RegSetValueExW(getKey().getHKEY(), getName().c_str(), 0, getType(), (BYTE*)&value, sizeof(value));
    return lastError == ERROR_SUCCESS;
}

Registry::ValueType Registry::DWORDValue::getType()
{
    return vtDWORD;
}

Registry::QWORDValue::QWORDValue(Key & key, const std::wstring & name)
    : Value(key, name)
{
}

UINT64 Registry::QWORDValue::get()
{
    UINT64 result;
    DWORD size = sizeof(result);
    lastError = RegGetValueW(getKey().getHKEY(), NULL, getName().c_str(), RRF_RT_ANY, NULL, &result, &size);
    if (lastError)
        return 0;
    return result;
}

bool Registry::QWORDValue::set(UINT64 value)
{
    lastError = RegSetValueExW(getKey().getHKEY(), getName().c_str(), 0, getType(), (BYTE*)&value, sizeof(value));
    return lastError == ERROR_SUCCESS;
}

Registry::ValueType Registry::QWORDValue::getType()
{
    return vtQWORD;
}
