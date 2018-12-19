// version 1.5

#include <GXCommon/Windows/GXThread.h>
#include <GXCommon/GXLogger.h>

GX_DISABLE_COMMON_WARNINGS

#include <process.h>

GX_RESTORE_WARNING_STATE


GXThread::GXThread ( PFNGXTHREADPROC procedure, GXVoid* argument ):
    GXAbstractThread ( procedure, argument )
{
    thread = INVALID_HANDLE_VALUE;
}

GXThread::~GXThread ()
{
    if ( thread == INVALID_HANDLE_VALUE || state != eGXThreadState::Started ) return;

    GXLogW ( L"GXThread::~GXThread::Warning - Поток завершён неверно\n" );
    system ( "pause" );
    CloseHandle ( thread );
}

GXVoid GXThread::Start ()
{
    if ( state == eGXThreadState::Started ) return;

    thread = reinterpret_cast<HANDLE> ( _beginthreadex ( nullptr, 0u, &GXThread::RootThreadStarter, this, 0, nullptr ) );

    if ( thread == INVALID_HANDLE_VALUE )
    {
        GXLogW ( L"GXThread::Start::Error - Не удалось создать поток\n" );
        return;
    }

    state = eGXThreadState::Started;
}

GXVoid GXThread::Switch ()
{
    SwitchToThread ();
}

GXVoid GXThread::Sleep ( GXUInt milliseconds )
{
    ::Sleep ( static_cast<DWORD> ( milliseconds ) );
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
    thread = INVALID_HANDLE_VALUE;
}

unsigned __stdcall GXThread::RootThreadStarter ( void* lpThreadParameter )
{
    GXThread* thread = reinterpret_cast<GXThread*> ( lpThreadParameter );
    GXUPointer result = thread->Procedure ( thread->argument, *thread );
    GXLogW ( L"GXThread::RootThreadStarter::Info - Поток 0x%p завершился с кодом %X\n", lpThreadParameter, result );

    return 0u;
}
