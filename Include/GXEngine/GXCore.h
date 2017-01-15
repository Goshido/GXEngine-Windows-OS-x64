//version 1.11

#ifndef GX_CORE
#define GX_CORE


#include <GXCommon/GXTypes.h>


typedef GXVoid ( GXCALL* PFNGXONGAMEINITPROC ) ();
typedef GXVoid ( GXCALL* PFNGXONGAMECLOSEPROC ) ();


class GXCore
{
	private:
		static GXBool					loopFlag;
		static GXCore*					instance;

	public:
		~GXCore ();

		GXVoid Start ( PFNGXONGAMEINITPROC onGameInit, PFNGXONGAMECLOSEPROC onGameClose );
		GXVoid Exit ();

		static GXCore* GXCALL GetInstance ();

	private:
		explicit GXCore ();
		GXVoid CheckMemoryLeak ();
};


#endif //GX_CORE
