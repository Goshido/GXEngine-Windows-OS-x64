//version 1.0

#include <GXCommon/GXThread.h>


GXThread::GXThread ( PFNGXTHREADPROC threadFunc, GXVoid* arg, eGXThreadState startState )
{
	state = startState;

	switch ( state )
	{
		case GX_SUSPEND:
			thread = CreateThread ( 0, 0, threadFunc, arg, CREATE_SUSPENDED, &threadID );
		break;

		case GX_WORKING:
			thread = CreateThread ( 0, 0, threadFunc, arg, 0, &threadID );
		break;

		default:
		break;
	}
}

GXThread::~GXThread ()
{
	CloseHandle ( thread );
}

GXVoid GXThread::Suspend ()
{
	if ( state == GX_SUSPEND ) return;

	state = GX_SUSPEND;
	SuspendThread ( thread );
}

GXVoid GXThread::Resume ()
{
	if ( state == GX_WORKING ) return;

	ResumeThread ( thread );
	state = GX_WORKING;
}

GXVoid GXThread::Join ()
{
	WaitForSingleObject ( thread, INFINITE );
}