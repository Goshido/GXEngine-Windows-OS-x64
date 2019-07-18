// version 1.6

#ifndef GX_SOUND_CHANNEL
#define GX_SOUND_CHANNEL


#include "GXSoundEmitter.h"


class GXSoundChannel final : public GXMemoryInspector
{
    public:
        GXSoundChannel**    _top;
        GXSoundChannel*     _next;
        GXSoundChannel*     _previous;

    private:
        GXFloat             _volume;
        GXSoundEmitter*     _emitters;

    public:
        GXSoundChannel ();
        ~GXSoundChannel () override;

        GXVoid SetVolume ( GXFloat volumeLevel );
        GXVoid AddEmitter ( GXSoundEmitter* emitter );

        GXVoid Update ();

    private:
        GXSoundChannel ( const GXSoundChannel &other ) = delete;
        GXSoundChannel& operator = ( const GXSoundChannel &other ) = delete;
};


#endif // GX_SOUND_CHANNEL
