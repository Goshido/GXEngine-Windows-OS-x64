// version 1.8

#ifndef GX_LOCALE
#define GX_LOCALE


#include <GXCommon/GXMemory.h>


enum class eGXLanguage : GXUShort
{
    English = 0u,
    Russian = 1u
};

class GXLocale final : public GXMemoryInspector
{
    private:
        eGXLanguage         _currentLanguage;
        GXDynamicArray      _storage;

        static GXLocale*    _instance;

    public:
        static GXLocale& GXCALL GetInstance ();
        ~GXLocale ();

        GXVoid LoadLanguage ( const GXWChar* fileName, eGXLanguage language );

        GXVoid SetLanguage ( eGXLanguage language );
        eGXLanguage GetLanguage () const;
    
        const GXWChar* GetString ( const GXWChar* resName ) const;

    private:
        GXLocale ();

        GXLocale ( const GXLocale &other ) = delete;
        GXLocale& operator = ( const GXLocale &other ) = delete;
};


#endif // GX_LOCALE
