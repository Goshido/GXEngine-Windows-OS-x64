// version 1.15

#ifndef GX_CORE
#define GX_CORE


#include "GXGame.h"


class GXCore
{
	private:
		static GXBool		loopFlag;
		static GXCore*		instance;

	public:
		static GXCore& GXCALL GetInstance ();
		~GXCore ();

		GXVoid Start ( GXGame &game );
		GXVoid Exit ();

	private:
		GXCore ();
		GXVoid CheckMemoryLeak ();

		GXCore ( const GXCore &other ) = delete;
		GXCore& operator = ( const GXCore &other ) = delete;
};


#endif // GX_CORE
