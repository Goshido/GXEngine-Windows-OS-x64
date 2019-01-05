// version 1.7

#ifndef GX_LOCALE
#define GX_LOCALE


#include <GXCommon/GXMemory.h>


enum class eGXLanguage : GXUShort
{
    English = 0,
    Russian = 1
};

class GXLocale final : public GXMemoryInspector
{
    private:
        eGXLanguage         currentLanguage;
        GXDynamicArray      storage;

        static GXLocale*    instance;

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
