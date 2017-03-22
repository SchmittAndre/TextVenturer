#pragma once

namespace Registry
{
    enum PredefinedKey
    {
        pkClassesRoot = reinterpret_cast<ULONG_PTR>(HKEY_CLASSES_ROOT), 
        pkCurrentConfig = reinterpret_cast<ULONG_PTR>(HKEY_CURRENT_CONFIG),
        pkCurrentUser = reinterpret_cast<ULONG_PTR>(HKEY_CURRENT_USER),
        pkCurrentUserLocalSettings = reinterpret_cast<ULONG_PTR>(HKEY_CURRENT_USER_LOCAL_SETTINGS),
        pkLocalMachine = reinterpret_cast<ULONG_PTR>(HKEY_LOCAL_MACHINE),
        pkPerformanceData = reinterpret_cast<ULONG_PTR>(HKEY_PERFORMANCE_DATA),
        pkPerformanceNLSText = reinterpret_cast<ULONG_PTR>(HKEY_PERFORMANCE_NLSTEXT),
        pkPerformanceText = reinterpret_cast<ULONG_PTR>(HKEY_PERFORMANCE_TEXT),
        pkUsers = reinterpret_cast<ULONG_PTR>(HKEY_USERS)
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
        Key(const std::wstring &path, PredefinedKey parent, CreationMode mode = cmReadOnly);
        Key(const std::wstring &path, Key& parent, CreationMode mode = cmReadOnly);
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

        explicit operator bool();  
    };

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
    };      

    class StringValue : public Value
    {
    public:
        StringValue(Key & key, const std::wstring &name);

        std::wstring get();
        bool set(std::wstring text);

        ValueType getType();
    };
}