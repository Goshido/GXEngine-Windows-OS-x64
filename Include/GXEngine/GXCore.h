//version 1.13

#ifndef GX_CORE
#define GX_CORE


#include "GXGame.h"


class GXCore
{
	private:
		static GXBool					loopFlag;
		static GXCore*					instance;

	public:
		~GXCore ();

		GXVoid Start ( GXGame &game );
		GXVoid Exit ();

		static GXCore& GXCALL GetInstance ();

	private:
		GXCore ();
		GXVoid CheckMemoryLeak ();
};


#endif //GX_CORE
