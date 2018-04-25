// version 1.2

#ifndef GX_THREAD
#define GX_THREAD


#include "GXTypes.h"


class GXThread;
typedef GXUPointer ( GXTHREADCALL* PFNGXTHREADPROC ) ( GXVoid* arg, GXThread& thread );


enum class eGXThreadState : GXUByte
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
		virtual GXVoid Switch () = 0;
		virtual GXVoid Join () = 0;

	private:
		GXAbstractThread ( const GXAbstractThread &other ) = delete;
		GXAbstractThread& operator = ( const GXAbstractThread &other ) = delete;
};


#ifdef __GNUC__
	#include "Posix/GXThread.h"
#else
	#include "Windows/GXThread.h"
#endif // __GNUC__


#endif // GX_THREAD
