//version 1.0

#ifndef GX_THREAD
#define GX_THREAD


#include "GXTypes.h"


typedef GXDword ( GXTHREADCALL* PFNGXTHREADPROC ) ( GXVoid* arg );


enum eGXThreadState
{
	GX_SUSPEND,
	GX_WORKING
};


class GXThread
{
	private:
		HANDLE			thread;
		DWORD			threadID;
		eGXThreadState	state;

	public:
		GXThread ( PFNGXTHREADPROC threadFunc, GXVoid* arg, eGXThreadState startState );
		~GXThread ();

		GXVoid Suspend ();
		GXVoid Resume ();
		GXVoid Join ();
};


#endif //GX_THREAD