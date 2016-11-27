//version 1.10

#ifndef GX_CORE
#define GX_CORE


#include "GXInput.h"
#include "GXRenderer.h"
#include "GXSoundMixer.h"
#include "GXNetwork.h"
#include "GXTouchSurface.h"
#include "GXLocale.h"


typedef GXVoid ( GXCALL* PFNGXONGAMEINITPROC ) ();
typedef GXVoid ( GXCALL* PFNGXONGAMECLOSEPROC ) ();


class GXCore
{
	private:
		static GXRenderer*				renderer;
		static GXInput*					input;
		static GXPhysics*				physics;
		static GXSoundMixer*			soundMixer;
		static GXNetServer*				server;
		static GXNetClient*				client;
		static GXTouchSurface*			touchSurface;
		static GXLocale*				locale;

		static GXBool					loopFlag;
		static PFNGXONGAMEINITPROC		OnGameInit;
		static PFNGXONGAMECLOSEPROC		OnGameClose;

		static GXCore*					instance;

	public:
		GXCore ( PFNGXONGAMEINITPROC onGameInit, PFNGXONGAMECLOSEPROC onGameClose, const GXWChar* gameName );
		~GXCore ();

		GXVoid Start ();
		GXVoid Exit ();

		WNDPROC NotifyGetInputProc ();

		GXRenderer* GetRenderer () const;
		GXPhysics* GetPhysics () const;
		GXInput* GetInput () const;
		GXSoundMixer* GetSoundMixer () const;
		GXNetServer* GetNetServer () const;
		GXNetClient* GetNetClient () const;
		GXTouchSurface* GetTouchSurface () const;
		GXLocale* GetLocale () const;

		static GXCore* GXCALL GetInstance ();

	private:
		GXVoid CheckMemoryLeak ();
};


#endif //GX_CORE