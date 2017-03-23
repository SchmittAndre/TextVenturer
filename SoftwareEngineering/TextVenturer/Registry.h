#pragma once

namespace Registry
{
    enum PredefinedKey
    {
        pkClassesRoot = reinterpret_cast<ULONG_PTR>(HKEY_CLASSES_ROOT), 
        pkCurrentUser = reinterpret_cast<ULONG_PTR>(HKEY_CURRENT_USER),
        pkLocalMachine = reinterpret_cast<ULONG_PTR>(HKEY_LOCAL_MACHINE),
        pkUsers = reinterpret_cast<ULONG_PTR>(HKEY_USERS),
        pkPerformanceData = reinterpret_cast<ULONG_PTR>(HKEY_PERFORMANCE_DATA),
        pkCurrentConfig = reinterpret_cast<ULONG_PTR>(HKEY_CURRENT_CONFIG),
        pkPerformanceText = reinterpret_cast<ULONG_PTR>(HKEY_PERFORMANCE_TEXT),
        pkPerformanceNLSText = reinterpret_cast<ULONG_PTR>(HKEY_PERFORMANCE_NLSTEXT),
        pkCurrentUserLocalSettings = reinterpret_cast<ULONG_PTR>(HKEY_CURRENT_USER_LOCAL_SETTINGS)
    };     

    enum ValueType
    {
        vtNone = REG_NONE,
        vtString = REG_SZ,
        vtExpandedString = REG_EXPAND_SZ,
        vtBinary = REG_BINARY,
        vtDWORD = REG_DWORD,
        vtDWORDLittleEndian = REG_DWORD_LITTLE_ENDIAN,
        vtDWORDBigEndian = REG_DWORD_BIG_ENDIAN,
        vtLink = REG_LINK,
        vtMultiString = REG_MULTI_SZ,
        vtResourceList = REG_RESOURCE_LIST,
        vtFullResourceDescriptor = REG_FULL_RESOURCE_DESCRIPTOR,
        vtResourceRequirementsList = REG_RESOURCE_REQUIREMENTS_LIST,
        vtQWORD = REG_QWORD,
        vtQWORDLittleEndian = REG_QWORD_LITTLE_ENDIAN
    };

    class Value;
    class Key
    {
        friend Value;
    public:
        enum State
        {
            ksError,
            ksMissing,
            ksCreated,
            ksOpened
        };

        enum CreationMode
        {
            cmReadOnly,
            cmReadWrite,
            cmCreate
        };

    private:
        HKEY handle;
        State state;
        LSTATUS lastError;

        std::vector<Value*> openValues;

        void constructor(const std::wstring &path, HKEY parent, CreationMode mode);

    public:
        Key(PredefinedKey parent);
        Key(PredefinedKey parent, const std::wstring &path, CreationMode mode = cmReadOnly);
        Key(Key& parent, const std::wstring &path, CreationMode mode = cmReadOnly);
        ~Key(); 

        bool isOpen();
        LSTATUS getLastError();
        State getState(); 
        bool exists();
        HKEY getHKEY();

        // values are valid, as long as they are in the same scope as the key
        Value* getValue(const std::wstring & name);
        Value* createValue(const std::wstring & name, ValueType type);
        void deleteValue(const std::wstring & name);

        Value* getDefaultValue();
        Value* createDefaultValue(ValueType type);
        void deleteDefaultValue();

        bool deleteKey(const std::wstring & name);

        explicit operator bool();  
    };

    
    class StringValue;
    class MultiStringValue;
    class DWORDValue;
    class QWORDValue;

    class Value abstract
    {
    private:
        Key & key;
        std::wstring name;

    protected:
        LSTATUS lastError;

        Key & getKey();
        const std::wstring & getName();

    public:
        Value(Key & key, const std::wstring &name);
        virtual ~Value();

        LSTATUS getLastError();

        virtual ValueType getType() = 0;

        operator StringValue*();
        operator MultiStringValue*();
        operator DWORDValue*();
        operator QWORDValue*();
    };      

    class StringValue : public Value
    {
    public:
        StringValue(Key & key, const std::wstring &name);

        std::wstring get();
        bool set(std::wstring text);

        ValueType getType();
    };

    class MultiStringValue : public Value
    {
    public:
        MultiStringValue(Key & key, const std::wstring &name);

        std::vector<std::wstring> get();
        bool set(std::vector<std::wstring> list);

        ValueType getType();
    };

    class DWORDValue : public Value
    {
    public:
        DWORDValue(Key & key, const std::wstring &name);

        UINT32 get();
        bool set(UINT32 value);

        ValueType getType();
    };

    class QWORDValue : public Value
    {
    public:
        QWORDValue(Key & key, const std::wstring &name);

        UINT64 get();
        bool set(UINT64 value);

        ValueType getType();
    };
}