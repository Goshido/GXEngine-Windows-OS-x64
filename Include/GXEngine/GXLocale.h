// version 1.10

#ifndef GX_LOCALE
#define GX_LOCALE


#include <GXCommon/GXStrings.h>
#include <GXCommon/GXAVLTree.h>


enum class eGXLanguage : GXUShort
{
    English = 0u,
    Russian = 1u
};

class GXLanguageNode;
class GXLanguageTree final : public GXAVLTree
{
    public:
        GXLanguageTree ();
        ~GXLanguageTree () override;

        // Method creates GXLanguageNode if node with "language" does not exist.
        GXLanguageNode& GetLanguage ( eGXLanguage language );

    private:
        static eGXCompareResult GXCALL Compare ( const GXAVLTreeNode &a, const GXAVLTreeNode &b );

        GXLanguageTree ( const GXLanguageTree &other ) = delete;
        GXLanguageTree& operator = ( const GXLanguageTree &other ) = delete;
};

//---------------------------------------------------------------------------------------------------------------------

class GXLocale final : public GXMemoryInspector
{
    private:
        const GXLanguageNode*       _currentLanguage;
        GXLanguageTree              _languages;

        static GXLocale*            _instance;

    public:
        static GXLocale& GXCALL GetInstance ();
        ~GXLocale ();

        GXVoid LoadLanguage ( GXString fileName, eGXLanguage language );

        GXVoid SetLanguage ( eGXLanguage language );
        eGXLanguage GetLanguage () const;

        GXString GetString ( GXString key ) const;

    private:
        GXLocale ();

        GXLocale ( const GXLocale &other ) = delete;
        GXLocale& operator = ( const GXLocale &other ) = delete;
};


#endif // GX_LOCALE
