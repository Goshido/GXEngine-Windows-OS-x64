//version 1.1

#ifndef GX_THREAD
#define GX_THREAD


#include "GXTypes.h"


typedef GXDword ( GXTHREADCALL* PFNGXTHREADPROC ) ( GXVoid* arg );


enum class eGXThreadState
{
	Waiting,
	Started
};


class GXThread
{
	private:
		HANDLE				thread;
		eGXThreadState		state;
		PFNGXTHREADPROC		threadProc;
		GXVoid*				arg;

	public:
		GXThread ( PFNGXTHREADPROC threadProc, GXVoid* arg );
		~GXThread ();

		GXVoid Start ();
		GXVoid Join ();

		eGXThreadState GetState () const;
};


#endif //GX_THREAD
