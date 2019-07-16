//version 1.3

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXOpenAL.h>
#include <GXCommon/GXLogger.h>


static ALCdevice*      gx_al_ptrALCDevice = nullptr;
static ALCcontext*     gx_al_ptrALCContext = nullptr;

GXVoid GXCALL GXOpenALDestroy ()
{
    alcMakeContextCurrent ( nullptr );
    alcDestroyContext ( gx_al_ptrALCContext );
    alcCloseDevice ( gx_al_ptrALCDevice );
}

ALboolean GXCALL GXOpenALCheckContextError ()
{
    const ALenum code = alcGetError ( gx_al_ptrALCDevice );

    if ( code == ALC_NO_ERROR )
        return AL_TRUE;

    const GXChar* errorString = static_cast<const GXChar*> ( alcGetString ( gx_al_ptrALCDevice, code ) );
    GXLogA ( "GXOpenALCheckContextError::Error - alcGetError post error: %s\n", errorString );
    return AL_FALSE;
}

ALboolean GXCALL GXOpenALCheckError ()
{
    const ALenum code = alGetError ();

    if ( code == AL_NO_ERROR )
        return AL_TRUE;

    const GXChar* errorString = static_cast<const GXChar*> ( alGetString ( code ) );
    GXLogA ( "GXOpenALCheckError::Error - alcGetError post error: %s\n", errorString );
    return AL_FALSE;
}

GXDLLEXPORT GXBool GXCALL GXOpenALInit ( GXOpenALFunctions &out )
{
    constexpr ALfloat ListenerPos[ 3u ] = { 0.0f, 0.0f, 0.0f };
    constexpr ALfloat ListenerVel[ 3u ] = { 0.0f, 0.0f, 0.0f };

    // ќриентаци€ слушател€. ( ѕервые 3 элемента Ц направление Ђнаї, последние 3 Ц Ђвверхї )
    constexpr ALfloat ListenerOri[ 6u ] = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
    gx_al_ptrALCDevice = alcOpenDevice ( nullptr );

    if ( !gx_al_ptrALCDevice )
    {
        GXLogW ( L"GXInitOpenAL::Error - «вукового устройства по умолчанию не существует" );
        return GX_FALSE;
    }

    gx_al_ptrALCContext = alcCreateContext ( gx_al_ptrALCDevice, NULL );
    alcMakeContextCurrent ( gx_al_ptrALCContext );

    // ”станавливаем параметры слушател€
    alListenerfv ( AL_POSITION, ListenerPos );
    alListenerfv ( AL_VELOCITY, ListenerVel );
    alListenerfv ( AL_ORIENTATION, ListenerOri );
    alDistanceModel ( AL_LINEAR_DISTANCE );

    *out.alBufferData = &alBufferData;
    *out.alBufferi = &alBufferi;
    *out.alDeleteBuffers = &alDeleteBuffers;
    *out.alDeleteSources = &alDeleteSources;
    *out.alGenBuffers = &alGenBuffers;
    *out.alGetSourcei = &alGetSourcei;
    *out.alGenSources = &alGenSources;
    *out.alListener3f = &alListener3f;
    *out.alListenerf = &alListenerf;
    *out.alListenerfv = &alListenerfv;
    *out.alSource3f = &alSource3f;
    *out.alSourcef = &alSourcef;
    *out.alSourcefv = &alSourcefv;
    *out.alSourcei = &alSourcei;
    *out.alSourcePause = &alSourcePause;
    *out.alSourcePlay = &alSourcePlay;
    *out.alSourceQueueBuffers = &alSourceQueueBuffers;
    *out.alSourceRewind = &alSourceRewind;
    *out.alSourceStop = &alSourceStop;
    *out.alSourceUnqueueBuffers = &alSourceUnqueueBuffers;
    *out.GXOpenALCheckContextError = &GXOpenALCheckContextError;
    *out.GXOpenALCheckError = &GXOpenALCheckError;
    *out.GXOpenALDestroy = &GXOpenALDestroy;

    return GX_TRUE;
}
