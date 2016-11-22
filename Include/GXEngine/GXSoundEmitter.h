//version 1.4

#ifndef GX_SOUND_EMITTER
#define GX_SOUND_EMITTER


#include "GXSoundProvider.h"
#include <GXCommon/GXMath.h>


class GXSoundEmitter
{
	public:
		GXSoundEmitter**	top;
		GXSoundEmitter*		next;
		GXSoundEmitter*		prev;

	private:
		GXSoundTrack*		track;
		GXSoundStreamer*	streamer;

		GXFloat				ownVolume;
		GXFloat				channelVolume;

		GXBool				looped;
		GXBool				finished;
		GXBool				stopped;

		GXBool				forceUpdate;

		ALuint				source;
		ALuint				streamBuffers[ 2 ];

	public:
		GXSoundEmitter ( GXSoundTrack* track, GXBool looped, GXBool streamed, GXBool isRelative );
		~GXSoundEmitter ();

		GXVoid SetVelocity ( const GXVec3 &velocity );
		GXVoid SetVelocity ( GXFloat x, GXFloat y, GXFloat z );

		GXVoid SetLocation ( const GXVec3 &location );
		GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );

		GXVoid SetRotation ( const GXMat4 &rotation );
		GXVoid SetRotation ( const GXVec3 &rotation );
		GXVoid SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad );

		GXVoid SetOwnVolume ( GXFloat ownVolume );
		GXVoid SetChannelVolume ( GXFloat channelVolume );

		GXVoid SetRange ( GXFloat min, GXFloat max );

		GXVoid ChangeSoundTrack ( GXSoundTrack* track, GXBool looped, GXBool streamed, GXBool isRelative );

		GXSoundTrack* GetSoundTrack ();

		GXVoid Play ();
		GXVoid Stop ();
		GXVoid Pause ();
		GXVoid Rewind ();

		GXVoid Update ();
};


#endif	//GX_SOUND_EMMITER