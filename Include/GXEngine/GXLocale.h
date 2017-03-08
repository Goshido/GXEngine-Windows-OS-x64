//version 1.2

#ifndef GX_LOCALE
#define GX_LOCALE


#include <GXCommon/GXMemory.h>


enum class eGXLanguage : GXShort
{
	English,
	Russian
};

class GXLocale
{
	private:
		eGXLanguage			language;
		GXDynamicArray		storage;

		static GXLocale*	instance;

	public:
		~GXLocale ();

		GXVoid LoadLanguage ( const GXWChar* fileName, eGXLanguage language );

		GXVoid SetLanguage ( eGXLanguage language );
		eGXLanguage GetLanguage () const;
	
		const GXWChar* GetString ( const GXWChar* resName ) const;

		static GXLocale* GXCALL GetInstance ();

	private:
		explicit GXLocale ();
};


#endif //GX_LOCALE
