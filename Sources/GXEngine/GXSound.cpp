// version 1.3

#include <GXEngine/GXSound.h>


extern HMODULE gx_GXEngineDLLModuleHandle;

//--------------------------------------------------------------------------

PFNALLISTENERFV                 GXAlListenerfv                  = nullptr;
PFNALLISTENER3F                 GXAlListener3f                  = nullptr;
PFNALLISTENERF                  GXAlListenerf                   = nullptr;

PFNALGENBUFFERS                 GXAlGenBuffers                  = nullptr;
PFNALDELETEBUFFERS              GXAlDeleteBuffers               = nullptr;

PFNALBUFFERI                    GXAlBufferi                     = nullptr;
PFNALBUFFERDATA                 GXAlBufferData                  = nullptr;

PFNALGENSOURCES                 GXAlGenSources                  = nullptr;
PFNALGETSOURCEI                 GXAlGetSourcei                  = nullptr;
PFNALDELETESOURCES              GXAlDeleteSources               = nullptr;

PFNALSOURCEI                    GXAlSourcei                     = nullptr;
PFNALSOURCEFV                   GXAlSourcefv                    = nullptr;
PFNALSOURCE3F                   GXAlSource3f                    = nullptr;
PFNALSOURCEF                    GXAlSourcef                     = nullptr;
PFNALSOURCEQUEUEBUFFERS         GXAlSourceQueueBuffers          = nullptr;
PFNALSOURCEUNQUEUEBUFFERS       GXAlSourceUnqueueBuffers        = nullptr;

PFNALSOURCEPLAY                 GXAlSourcePlay                  = nullptr;
PFNALSOURCESTOP                 GXAlSourceStop                  = nullptr;
PFNALSOURCEPAUSE                GXAlSourcePause                 = nullptr;
PFNALSOURCEREWIND               GXAlSourceRewind                = nullptr;

PFNGXOPENALCHECKERROR           GXOpenALCheckError              = nullptr;
PFNGXOPENALCHECKCONTEXTERROR    GXOpenALCheckContextError       = nullptr;
PFNGXOPENALDESTROY              GXOpenALDestroy                 = nullptr;

//--------------------------------------------------------------------------

PFNOVOPENCALLBACKS              GXOvOpenCallbacks               = nullptr;
PFNOVREAD                       GXOvRead                        = nullptr;
PFNOVPCMSEEK                    GXOvPcmSeek                     = nullptr;
PFNOVPCMTOTAL                   GXOvPcmTotal                    = nullptr;
PFNOVCLEAR                      GXOvClear                       = nullptr;

GXBool GXCALL GXSoundInit ()
{
    gx_GXEngineDLLModuleHandle = LoadLibraryW ( L"GXEngine.dll" );

    if ( !gx_GXEngineDLLModuleHandle )
    {
        GXLogA ( "GXPhysXUnit::Error - Не удалось загрузить GXEngine.dll\n" );
        return GX_FALSE;
    }

    PFNGXOGGVORBISINIT GXOGGVorbisInit;

    GXOGGVorbisInit = reinterpret_cast<PFNGXOGGVORBISINIT> ( reinterpret_cast<GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXOGGVorbisInit" ) ) );

    if ( !GXOGGVorbisInit )
    {
        GXLogA ( "GXSoundInit::Error - Не удалось найти функцию GXOGGVorbisInit\n" );
        return GX_FALSE;
    }

    GXOGGVorbisFunctions oggVorbisOut;
    oggVorbisOut.ov_clear = &GXOvClear;
    oggVorbisOut.ov_open_callbacks = &GXOvOpenCallbacks;
    oggVorbisOut.ov_pcm_seek = &GXOvPcmSeek;
    oggVorbisOut.ov_pcm_total = &GXOvPcmTotal;
    oggVorbisOut.ov_read = &GXOvRead;

    GXOGGVorbisInit ( oggVorbisOut );

    PFNGXOPENALINIT GXOpenALInit;
    GXOpenALInit = reinterpret_cast<PFNGXOPENALINIT> ( reinterpret_cast<GXVoid*> ( GetProcAddress ( gx_GXEngineDLLModuleHandle, "GXOpenALInit" ) ) );

    if ( !GXOpenALInit )
    {
        GXLogA ( "GXSoundInit::Error - Не удалось найти функцию GXOpenALInit\n" );
        return GX_FALSE;
    }

    GXOpenALFunctions alOut;
    alOut.alBufferData = &GXAlBufferData;
    alOut.alBufferi = &GXAlBufferi;
    alOut.alDeleteBuffers = &GXAlDeleteBuffers;
    alOut.alDeleteSources = &GXAlDeleteSources;
    alOut.alGenBuffers = &GXAlGenBuffers;
    alOut.alGenSources = &GXAlGenSources;
    alOut.alGetSourcei = &GXAlGetSourcei;
    alOut.alListener3f = &GXAlListener3f;
    alOut.alListenerf = &GXAlListenerf;
    alOut.alListenerfv = &GXAlListenerfv;
    alOut.alSource3f = &GXAlSource3f;
    alOut.alSourcef = &GXAlSourcef;
    alOut.alSourcefv = &GXAlSourcefv;
    alOut.alSourcei = &GXAlSourcei;
    alOut.alSourcePause = &GXAlSourcePause;
    alOut.alSourcePlay = &GXAlSourcePlay;
    alOut.alSourceQueueBuffers = &GXAlSourceQueueBuffers;
    alOut.alSourceRewind = &GXAlSourceRewind;
    alOut.alSourceStop = &GXAlSourceStop;
    alOut.alSourceUnqueueBuffers = &GXAlSourceUnqueueBuffers;
    alOut.GXOpenALCheckContextError = &GXOpenALCheckContextError;
    alOut.GXOpenALCheckError = &GXOpenALCheckError;
    alOut.GXOpenALDestroy = &GXOpenALDestroy;

    if ( !GXOpenALInit ( alOut ) )
    {
        GXLogA ( "GXSoundInit::Error - GXOpenALInit провалена\n" );
        return GX_FALSE;
    }

    return GX_TRUE;
}

GXBool GXCALL GXSoundDestroy ()
{
    if ( !gx_GXEngineDLLModuleHandle )
    {
        GXLogA ( "GXSoundDestroy::Error - Попытка выгрузить несуществующую в памяти GXEngine.dll\n" );
        return GX_FALSE;
    }

    if ( !GXOpenALDestroy )
    {
        GXLogA ( "GXSoundInit::Error - Не удалось найти функцию GXOGGVorbisInit\n" );
        return GX_FALSE;
    }

    GXOpenALDestroy ();

    if ( !FreeLibrary ( gx_GXEngineDLLModuleHandle ) )
    {
        GXLogA ( "GXSoundDestroy::Error - Не удалось выгрузить библиотеку GXEngine.dll\n" );
        return GX_FALSE;
    }

    return GX_TRUE;
}