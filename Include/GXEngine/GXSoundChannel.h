// version 1.2

#ifndef GX_SOUND_CHANNEL
#define GX_SOUND_CHANNEL


#include "GXSoundEmitter.h"


class GXSoundChannel
{
	public:
		GXSoundChannel**	top;
		GXSoundChannel*		next;
		GXSoundChannel*		prev;

	private:
		GXFloat				volume;
		GXSoundEmitter*		emitters;

	public:
		GXSoundChannel ();
		~GXSoundChannel ();

		GXVoid SetVolume ( GXFloat volumeLevel );
		GXVoid AddEmitter ( GXSoundEmitter* emitter );

		GXVoid Update ();

	private:
		GXSoundChannel ( const GXSoundChannel &other ) = delete;
		GXSoundChannel& operator = ( const GXSoundChannel &other ) = delete;
};


#endif // GX_SOUND_CHANNEL
