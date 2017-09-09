//version 1.1

#ifndef GX_OPEN_AL
#define GX_OPEN_AL


#include <GXCommon/GXDisable3rdPartyWarnings.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <mmreg.h>
#include <GXCommon/GXRestoreWarnings.h>


typedef void ( AL_APIENTRY* PFNALLISTENERFV ) ( ALenum param, const ALfloat* values );
typedef void ( AL_APIENTRY* PFNALLISTENER3F ) ( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
typedef void ( AL_APIENTRY* PFNALLISTENERF ) ( ALenum param, ALfloat value );

typedef void ( AL_APIENTRY* PFNALGENBUFFERS ) ( ALsizei n, ALuint* buffers );
typedef void ( AL_APIENTRY* PFNALDELETEBUFFERS ) ( ALsizei n, const ALuint* buffers );

typedef void ( AL_APIENTRY* PFNALBUFFERI ) ( ALuint bid, ALenum param, ALint value );
typedef void ( AL_APIENTRY* PFNALBUFFERDATA ) ( ALuint bid, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq );

typedef void ( AL_APIENTRY* PFNALGENSOURCES ) ( ALsizei n, ALuint* sources ); 
typedef void ( AL_APIENTRY* PFNALGETSOURCEI ) ( ALuint sid,  ALenum param, ALint* value );
typedef void ( AL_APIENTRY* PFNALDELETESOURCES ) ( ALsizei n, const ALuint* sources );

typedef void ( AL_APIENTRY* PFNALSOURCEI ) ( ALuint sid, ALenum param, ALint value ); 
typedef void ( AL_APIENTRY* PFNALSOURCEFV ) ( ALuint sid, ALenum param, const ALfloat* values );
typedef void ( AL_APIENTRY* PFNALSOURCE3F ) ( ALuint sid, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
typedef void ( AL_APIENTRY* PFNALSOURCEF ) ( ALuint sid, ALenum param, ALfloat value );
typedef void ( AL_APIENTRY* PFNALSOURCEQUEUEBUFFERS ) ( ALuint sid, ALsizei numEntries, const ALuint *bids );
typedef void ( AL_APIENTRY* PFNALSOURCEUNQUEUEBUFFERS ) ( ALuint sid, ALsizei numEntries, ALuint *bids );

typedef void ( AL_APIENTRY* PFNALSOURCEPLAY ) ( ALuint sid );
typedef void ( AL_APIENTRY* PFNALSOURCESTOP ) ( ALuint sid );
typedef void ( AL_APIENTRY* PFNALSOURCEPAUSE ) ( ALuint sid );
typedef void ( AL_APIENTRY* PFNALSOURCEREWIND ) ( ALuint sid );

typedef GXVoid ( GXCALL* PFNGXOPENALDESTROY ) ();
typedef ALboolean ( GXCALL* PFNGXOPENALCHECKCONTEXTERROR ) ();
typedef ALboolean ( GXCALL* PFNGXOPENALCHECKERROR ) ();


struct GXOpenALFunctions
{
	PFNALLISTENERFV*				alListenerfv;
	PFNALLISTENER3F*				alListener3f;
	PFNALLISTENERF*					alListenerf;

	PFNALGENBUFFERS*				alGenBuffers;
	PFNALDELETEBUFFERS*				alDeleteBuffers;

	PFNALBUFFERI*					alBufferi;
	PFNALBUFFERDATA*				alBufferData;
	
	PFNALGENSOURCES*				alGenSources;
	PFNALGETSOURCEI*				alGetSourcei;
	PFNALDELETESOURCES*				alDeleteSources;

	PFNALSOURCEI*					alSourcei;
	PFNALSOURCEFV*					alSourcefv;
	PFNALSOURCE3F*					alSource3f;
	PFNALSOURCEF*					alSourcef;
	PFNALSOURCEQUEUEBUFFERS*		alSourceQueueBuffers;
	PFNALSOURCEUNQUEUEBUFFERS*		alSourceUnqueueBuffers;

	PFNALSOURCEPLAY*				alSourcePlay;
	PFNALSOURCESTOP*				alSourceStop;
	PFNALSOURCEPAUSE*				alSourcePause;
	PFNALSOURCEREWIND*				alSourceRewind;

	PFNGXOPENALDESTROY*				GXOpenALDestroy;
	PFNGXOPENALCHECKCONTEXTERROR*	GXOpenALCheckContextError;
	PFNGXOPENALCHECKERROR*			GXOpenALCheckError;
};


#endif //GX_OPEN_AL
