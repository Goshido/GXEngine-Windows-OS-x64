// version 1.10

#ifndef GX_SOUND_EMITTER
#define GX_SOUND_EMITTER


#include "GXSoundProvider.h"
#include <GXCommon/GXMath.h>


class GXSoundEmitter final : public GXMemoryInspector
{
    public:
        GXSoundEmitter**    _top;
        GXSoundEmitter*     _next;
        GXSoundEmitter*     _previous;

    private:
        GXSoundTrack*       _track;
        GXSoundStreamer*    _streamer;

        GXFloat             _ownVolume;
        GXFloat             _channelVolume;

        GXBool              _looped;
        GXBool              _finished;
        GXBool              _stopped;

        GXBool              _forceUpdate;

        ALuint              _source;
        ALuint              _streamBuffers[ 2u ];

    public:
        explicit GXSoundEmitter ( GXSoundTrack* track, GXBool isTrackLooped, GXBool isTrackStreamed, GXBool isRelative );
        ~GXSoundEmitter () override;

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
