//version 1.1

#ifndef GX_SOUND
#define GX_SOUND


#include <GXEngineDLL/GXEngineAPI.h>


extern PFNALLISTENERFV				GXAlListenerfv;
extern PFNALLISTENER3F				GXAlListener3f;
extern PFNALLISTENERF				GXAlListenerf;

extern PFNALGENBUFFERS				GXAlGenBuffers;
extern PFNALDELETEBUFFERS			GXAlDeleteBuffers;

extern PFNALBUFFERI					GXAlBufferi;
extern PFNALBUFFERDATA				GXAlBufferData;

extern PFNALGENSOURCES				GXAlGenSources;
extern PFNALGETSOURCEI				GXAlGetSourcei;
extern PFNALDELETESOURCES			GXAlDeleteSources;

extern PFNALSOURCEI					GXAlSourcei;
extern PFNALSOURCEFV				GXAlSourcefv;
extern PFNALSOURCE3F				GXAlSource3f;
extern PFNALSOURCEF					GXAlSourcef;
extern PFNALSOURCEQUEUEBUFFERS		GXAlSourceQueueBuffers;
extern PFNALSOURCEUNQUEUEBUFFERS	GXAlSourceUnqueueBuffers;

extern PFNALSOURCEPLAY				GXAlSourcePlay;
extern PFNALSOURCESTOP				GXAlSourceStop;
extern PFNALSOURCEPAUSE				GXAlSourcePause;
extern PFNALSOURCEREWIND			GXAlSourceRewind;

extern PFNGXOPENALCHECKERROR		GXOpenALCheckError;
extern PFNGXOPENALCHECKCONTEXTERROR	GXOpenALCheckContextError;

//--------------------------------------------------------------------------

extern PFNOVOPENCALLBACKS			GXOvOpenCallbacks;
extern PFNOVREAD					GXOvRead;
extern PFNOVPCMSEEK					GXOvPcmSeek;
extern PFNOVCLEAR					GXOvClear;
extern PFNOVPCMTOTAL				GXOvPcmTotal;

//--------------------------------------------------------------------------

GXBool GXCALL GXSoundInit ();
GXBool GXCALL GXSoundDestroy ();


#endif //GXSOUND