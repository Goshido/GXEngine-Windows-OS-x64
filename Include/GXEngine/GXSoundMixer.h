//version 1.6

#ifndef GX_SOUND_MIXER
#define GX_SOUND_MIXER


#include "GXSoundChannel.h"
#include <GXCommon/GXThread.h>
#include <GXCommon/GXMutex.h>


class GXSoundMixer
{
	private:
		static GXBool				loopFlag;
		GXThread					thread;

		GXFloat						masterVolume;

		static GXSoundChannel*		channels;

		static GXSoundMixer*		instance;

	public:
		static GXSoundMixer& GXCALL GetInstance ();
		~GXSoundMixer ();

		GXVoid SetListenerVelocity ( const GXVec3 &velocity );
		GXVoid SetListenerVelocity ( GXFloat x, GXFloat y, GXFloat z );

		GXVoid SetListenerLocation ( const GXVec3 &location );
		GXVoid SetListenerLocation ( GXFloat x, GXFloat y, GXFloat z );

		GXVoid SetListenerRotation ( const GXMat4 &rotation );
		GXVoid SetListenerRotation ( const GXEuler &rotation );
		GXVoid SetListenerRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

		GXVoid AddChannel ( GXSoundChannel* channel );

		GXVoid Start ();
		GXBool Shutdown ();

		GXVoid SetMasterVolume ( GXFloat masterVolumeLevel );

	private:
		GXSoundMixer ();

		static GXUPointer GXTHREADCALL Update ( GXVoid* args, GXThread &thread );
};


#endif	// GX_SOUND_MIXER