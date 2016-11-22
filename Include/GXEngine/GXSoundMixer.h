//version 1.2

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

	public:
		GXSoundMixer ();
		~GXSoundMixer ();

		GXVoid SetListenerVelocity ( const GXVec3 &velocity );
		GXVoid SetListenerVelocity ( GXFloat x, GXFloat y, GXFloat z );

		GXVoid SetListenerLocation ( const GXVec3 &location );
		GXVoid SetListenerLocation ( GXFloat x, GXFloat y, GXFloat z );

		GXVoid SetListenerRotation ( const GXMat4 &rotation );
		GXVoid SetListenerRotation ( const GXVec3 &rotation );
		GXVoid SetListenerRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );

		GXVoid AddChannel ( GXSoundChannel* channel );

		GXVoid Start ();
		GXVoid Suspend ();
		GXBool Shutdown ();

		GXVoid SetMasterVolume ( GXFloat masterVolume );

	private:
		static GXDword GXTHREADCALL Update ( GXVoid* args );

};


#endif	// GX_SOUND_MIXER