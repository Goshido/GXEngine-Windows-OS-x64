//version 1.1

#include <GXCommon/GXThread.h>
#include <GXCommon/GXLogger.h>


GXThread::GXThread ( PFNGXTHREADPROC threadProc, GXVoid* arg )
{
	this->threadProc = threadProc;
	this->arg = arg;
	this->state = eGXThreadState::Waiting;

	thread = NULL;
}

GXThread::~GXThread ()
{
	CloseHandle ( thread );
}

GXVoid GXThread::Start ()
{
	if ( state == eGXThreadState::Started ) return;

	DWORD threadID = 0;
	thread = CreateThread ( nullptr, 0, threadProc, arg, 0, &threadID );

	if ( thread == NULL )
		GXLogW ( L"GXThread::Start::Error - Не удалось создать поток\n" );
	else
		state = eGXThreadState::Started;
}

GXVoid GXThread::Join ()
{
	if ( state == eGXThreadState::Waiting )
	{
		GXLogW ( L"GXThread::Join::Warning - Поток не запущен\n" );
		return;
	}

	WaitForSingleObject ( thread, INFINITE );
	CloseHandle ( thread );
	thread = NULL;
}

eGXThreadState GXThread::GetState () const
{
	return state;
}
