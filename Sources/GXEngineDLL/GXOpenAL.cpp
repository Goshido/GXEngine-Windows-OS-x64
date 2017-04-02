//version 1.0

#include <GXEngineDLL/GXEngineDLL.h>
#include <GXEngineDLL/GXOpenAL.h>
#include <GXCommon/GXLogger.h>


ALCdevice*	gx_al_ptrALCDevice;
ALCcontext*	gx_al_ptrALCContext;


GXVoid GXCALL GXOpenALDestroy ()
{
	alcMakeContextCurrent ( 0 );
	alcDestroyContext ( gx_al_ptrALCContext );
	alcCloseDevice ( gx_al_ptrALCDevice );
}

ALboolean GXCALL GXOpenALCheckContextError ()
{
	ALenum ErrCode;
	if ( ( ErrCode = alcGetError ( gx_al_ptrALCDevice ) ) != ALC_NO_ERROR )
	{
		GXChar* errorString = (GXChar*)alcGetString ( gx_al_ptrALCDevice, ErrCode );
		MessageBoxA ( 0, errorString, "OpenAL Context Error", 0 );
		GXLogA ( "OpenAL::Context Error - %s\n", errorString );
		return AL_FALSE;
	}

	return AL_TRUE;
}

ALboolean GXCALL GXOpenALCheckError ()
{
	ALenum ErrCode;
	if ( ( ErrCode = alGetError () ) != AL_NO_ERROR )
	{
		GXChar* errorString = (GXChar*)alGetString ( ErrCode );
		MessageBoxA ( 0, errorString, "OpenAL Error", 0 );
		GXLogA ( "OpenAL::Error - %s\n", errorString );
		return AL_FALSE;
	}

	return AL_TRUE;
}

GXDLLEXPORT GXBool GXCALL GXOpenALInit ( GXOpenALFunctions &out )
{
	ALfloat ListenerPos [] = { 0.0, 0.0, 0.0 };
	ALfloat ListenerVel [] = { 0.0, 0.0, 0.0 };
	// ќриентаци€ слушател€. ( ѕервые 3 элемента Ц направление Ђнаї, последние 3 Ц Ђвверхї )
	ALfloat ListenerOri [] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
	gx_al_ptrALCDevice = alcOpenDevice ( NULL );
	if ( !gx_al_ptrALCDevice )
	{
		GXDebugBox ( L"«вукового устройства по умолчанию не существует" );
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