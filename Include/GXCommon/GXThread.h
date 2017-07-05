// version 1.1

#ifndef GX_THREAD
#define GX_THREAD


#include "GXTypes.h"


typedef GXUPointer ( GXTHREADCALL* PFNGXTHREADPROC ) ( GXVoid* arg );


enum class eGXThreadState
{
	Waiting,
	Started
};


class GXAbstractThread
{
	protected:
		PFNGXTHREADPROC		Procedure;
		GXVoid*				argument;
		eGXThreadState		state;

	public:
		explicit GXAbstractThread ( PFNGXTHREADPROC procedure, GXVoid* argument );
		virtual ~GXAbstractThread ();

		eGXThreadState GetState () const;

		virtual GXVoid Start () = 0;
		virtual GXVoid Join () = 0;
};


#ifdef __GNUC__
	#include "Posix/GXThread.h"
#else
	#include "Windows/GXThread.h"
#endif //__GNUC__


#endif //GX_THREAD
