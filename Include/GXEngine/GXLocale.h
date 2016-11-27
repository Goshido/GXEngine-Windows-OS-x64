//version 1.1

#ifndef GX_LOCALE
#define GX_LOCALE


#include <GXCommon/GXMemory.h>


enum eGXLanguage : GXShort
{
	GX_LANGUAGE_EN = 0,
	GX_LANGUAGE_RU = 1
};

class GXLocale
{
	private:
		eGXLanguage			language;
		GXDynamicArray		storage;

		static GXLocale*	instance;

	public:
		GXLocale ();
		~GXLocale ();

		GXVoid LoadLanguage ( const GXWChar* fileName, eGXLanguage language );

		GXVoid SetLanguage ( eGXLanguage language );
		eGXLanguage GetLanguage () const;
	
		const GXWChar* GetString ( const GXWChar* resName ) const;

		static GXLocale* GXCALL GetInstance ();
};


#endif //GX_LOCALE
