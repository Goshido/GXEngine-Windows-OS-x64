//version 1.2

#include <GXCommon/Windows/GXThread.h>
#include <GXCommon/GXLogger.h>


GXThread::GXThread ( PFNGXTHREADPROC procedure, GXVoid* argument ) :
GXAbstractThread ( procedure, argument )
{
	thread = NULL;
}

GXThread::~GXThread ()
{
	if ( !thread == NULL && state == eGXThreadState::Started )
	{
		GXLogW ( L"GXThread::~GXThread::Warning - Поток завершён неверно\n" );
		system ( "pause" );
		CloseHandle ( thread );
	}
}

GXVoid GXThread::Start ()
{
	if ( state == eGXThreadState::Started ) return;

	DWORD threadID = 0;
	thread = CreateThread ( nullptr, 0, &GXThread::RootThreadStarter, this, 0, &threadID );

	if ( thread == NULL )
		GXLogW ( L"GXThread::Start::Error - Не удалось создать поток\n" );
	else
		state = eGXThreadState::Started;
}

GXVoid GXThread::Switch ()
{
	SwitchToThread ();
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

DWORD WINAPI GXThread::RootThreadStarter ( LPVOID lpThreadParameter )
{
	GXThread* thread = (GXThread*)lpThreadParameter;
	GXUPointer result = thread->Procedure ( thread->argument, *thread );
	GXLogW ( L"GXThread::RootThreadStarter::Info - Поток %X завершился с кодом %X\n", (GXUPointer)lpThreadParameter, result );

	return 0;
}
