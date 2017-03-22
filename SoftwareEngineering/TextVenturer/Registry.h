#pragma once

namespace Registry
{
    enum PredefinedKey
    {
        pkClassesRoot, 
        pkCurrentConfig,
        pkCurrentUser,
        pkCurrentUserLocalSettings,
        pkLocalMachine,
        pkPerformanceData,
        pkPerformanceNLSText,
        pkPerformanceText,
        pkUsers
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
            ksCreated,
            ksOpened
        };

    private:
        HKEY handle;
        State state;
        LSTATUS lastError;

        std::vector<Value*> openValues;

        static HKEY predefinedToHKEY(PredefinedKey key);
        void constructor(const std::wstring &path, HKEY parent, bool canCreate);

    public:
        Key(const std::wstring &path, PredefinedKey parent, bool canCreate = false);
        Key(const std::wstring &path, Key& parent, bool canCreate = false);
        ~Key(); 

        bool isOpen();
        LSTATUS getLastError();
        State getState(); 
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