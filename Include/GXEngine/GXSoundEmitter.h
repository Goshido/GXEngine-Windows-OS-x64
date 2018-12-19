// version 1.9

#ifndef GX_SOUND_EMITTER
#define GX_SOUND_EMITTER


#include "GXSoundProvider.h"
#include <GXCommon/GXMath.h>


class GXSoundEmitter final
{
    public:
        GXSoundEmitter**    top;
        GXSoundEmitter*     next;
        GXSoundEmitter*     prev;

    private:
        GXSoundTrack*       track;
        GXSoundStreamer*    streamer;

        GXFloat             ownVolume;
        GXFloat             channelVolume;

        GXBool              looped;
        GXBool              finished;
        GXBool              stopped;

        GXBool              forceUpdate;

        ALuint              source;
        ALuint              streamBuffers[ 2u ];

    public:
        explicit GXSoundEmitter ( GXSoundTrack* track, GXBool looped, GXBool streamed, GXBool isRelative );
        ~GXSoundEmitter ();

        GXVoid SetVelocity ( const GXVec3 &velocity );
        GXVoid SetVelocity ( GXFloat x, GXFloat y, GXFloat z );

        GXVoid SetLocation ( const GXVec3 &location );
        GXVoid SetLocation ( GXFloat x, GXFloat y, GXFloat z );

        GXVoid SetRotation ( const GXMat4 &rotation );
        GXVoid SetRotation ( const GXEuler &rotation );
        GXVoid SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians );

        GXVoid SetOwnVolume ( GXFloat ownVolumeLevel );
        GXVoid SetChannelVolume ( GXFloat channelVolumeLevel );

        GXVoid SetRange ( GXFloat min, GXFloat max );

        GXVoid ChangeSoundTrack ( GXSoundTrack* trackObject, GXBool isTrackLooped, GXBool isTrackStreamed, GXBool isRelative );

        GXSoundTrack* GetSoundTrack ();

        GXVoid Play ();
        GXVoid Stop ();
        GXVoid Pause ();
        GXVoid Rewind ();

        GXVoid Update ();

    private:
        GXSoundEmitter () = delete;
        GXSoundEmitter ( const GXSoundEmitter &other ) = delete;
        GXSoundEmitter& operator = ( const GXSoundEmitter &other ) = delete;
};


#endif // GX_SOUND_EMMITER
