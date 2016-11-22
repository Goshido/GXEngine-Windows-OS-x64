//version 1.0

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
		eGXLanguage		language;
		GXDynamicArray	storage;

	public:
		GXLocale ();
		~GXLocale ();

		GXVoid LoadLanguage ( const GXWChar* fileName, eGXLanguage language );

		GXVoid SetLanguage ( eGXLanguage language );
		eGXLanguage GetLanguage ();
	
		const GXWChar* GetString ( const GXWChar* resName );
};


#endif //GX_LOCALE
