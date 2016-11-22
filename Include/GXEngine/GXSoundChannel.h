//version 1.1

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

		GXVoid SetVolume ( GXFloat volume );
		GXVoid AddEmitter ( GXSoundEmitter* emitter );

		GXVoid Update ();
};


#endif	//GX_SOUND_CHANNEL