// version 1.3

#include <GXCommon/Windows/GXThread.h>
#include <GXCommon/GXLogger.h>


GXThread::GXThread ( PFNGXTHREADPROC procedure, GXVoid* argument ) :
GXAbstractThread ( procedure, argument )
{
	thread = NULL;
}

GXThread::~GXThread ()
{
	if ( thread == NULL || state != eGXThreadState::Started ) return;

	GXLogW ( L"GXThread::~GXThread::Warning - ����� �������� �������\n" );
	system ( "pause" );
	CloseHandle ( thread );
}

GXVoid GXThread::Start ()
{
	if ( state == eGXThreadState::Started ) return;

	DWORD threadID = 0;
	thread = CreateThread ( nullptr, 0, &GXThread::RootThreadStarter, this, 0, &threadID );

	if ( thread == NULL )
		GXLogW ( L"GXThread::Start::Error - �� ������� ������� �����\n" );
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
		GXLogW ( L"GXThread::Join::Warning - ����� �� �������\n" );
		return;
	}

	WaitForSingleObject ( thread, INFINITE );
	CloseHandle ( thread );
	thread = NULL;
}

DWORD WINAPI GXThread::RootThreadStarter ( LPVOID lpThreadParameter )
{
	GXThread* thread = reinterpret_cast<GXThread*> ( lpThreadParameter );
	GXUPointer result = thread->Procedure ( thread->argument, *thread );
	GXLogW ( L"GXThread::RootThreadStarter::Info - ����� 0x%p ���������� � ����� %X\n", lpThreadParameter, result );

	return 0;
}
