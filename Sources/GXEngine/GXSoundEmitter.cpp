// version 1.9

#include <GXEngine/GXSoundEmitter.h>
#include <GXCommon/GXSmartLock.h>


GXSmartLock* gx_sound_mixer_SmartLock = nullptr;

GXSoundEmitter::GXSoundEmitter ( GXSoundTrack* trackObject, GXBool isTrackLooped, GXBool isTrackStreamed, GXBool isRelative )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXSoundEmitter" )
{
    if ( !trackObject )
        GXWarningBox ( L"GXSoundEmitter::Error - track равен nullptr!" );

    gx_sound_mixer_SmartLock->AcquireExclusive ();

    top = nullptr;
    next = prev = nullptr;

    stopped = GX_TRUE;
    finished = GX_FALSE;

    forceUpdate = GX_FALSE;

    source = 0u;
    streamBuffers[ 0u ] = streamBuffers[ 1u ] = 0u;


    track = trackObject;
    looped = isTrackLooped;

    track->AddReference ();

    GXAlGenSources ( 1, &source );

    if ( isTrackStreamed )
    {
        streamer = track->GetStreamer ();
        GXAlGenBuffers ( 2, streamBuffers );

        streamer->FillBuffer ( streamBuffers[ 0u ], looped );
        streamer->FillBuffer ( streamBuffers[ 1u ], looped );
        GXAlSourceQueueBuffers ( source, 2, streamBuffers );
    }
    else
    {
        streamer = nullptr;
        GXAlSourcei ( source, AL_LOOPING, looped ? AL_TRUE : AL_FALSE );

        ALuint bfr = track->GetBuffer ();
        GXAlSourcei ( source, AL_BUFFER, static_cast<ALint> ( bfr ) );
    }

    ownVolume = 1.0f;
    SetChannelVolume ( 1.0f );

    GXAlSourcei ( source, AL_SOURCE_RELATIVE, isRelative ? AL_TRUE : AL_FALSE );

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXSoundEmitter::~GXSoundEmitter ()
{
    gx_sound_mixer_SmartLock->AcquireExclusive ();

    Stop ();
    GXAlDeleteSources ( 1, &source );
    
    if ( streamer )
    {
        GXAlSourceUnqueueBuffers ( source, 1, streamBuffers );
        GXAlSourceUnqueueBuffers ( source, 1, streamBuffers + 1u );
        GXAlDeleteBuffers ( 2, streamBuffers );

        delete streamer;
    }

    track->Release ();

    if ( top )
    {
        if ( next )
            next->prev = prev;

        if ( prev )
        {
            prev->next = next;
        }
        else
        {
            *top = next;
        }
    }

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXVoid GXSoundEmitter::SetVelocity ( const GXVec3 &velocity )
{
    GXAlSourcefv ( source, AL_VELOCITY, velocity.data );
}

GXVoid GXSoundEmitter::SetVelocity ( GXFloat x, GXFloat y, GXFloat z )
{
    GXAlSource3f ( source, AL_VELOCITY, x, y, z );
}

GXVoid GXSoundEmitter::SetLocation ( const GXVec3 &location )
{
    GXAlSourcefv ( source, AL_POSITION, location.data );
}

GXVoid GXSoundEmitter::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    GXAlSource3f ( source, AL_POSITION, x, y, z );
}

GXVoid GXSoundEmitter::SetRotation ( const GXMat4 &rotation )
{
    GXFloat orientation[ 6u ];
    
    GXVec3 tmp;
    rotation.GetZ ( tmp );
    tmp.Reverse ();
    memcpy ( orientation, &tmp, sizeof ( GXVec3 ) );

    rotation.GetY ( tmp );
    memcpy ( orientation + 3u, &tmp, sizeof ( GXVec3 ) );

    GXAlSourcefv ( source, AL_ORIENTATION, orientation );
}

GXVoid GXSoundEmitter::SetRotation ( const GXEuler &rotation )
{
    GXMat4 orientation;
    orientation.RotationXYZ ( rotation.pitchRadians, rotation.yawRadians, rotation.rollRadians );

    SetRotation ( orientation );
}

GXVoid GXSoundEmitter::SetRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
    GXMat4 orientation;
    orientation.RotationXYZ ( pitchRadians, yawRadians, rollRadians );

    SetRotation ( orientation );
}

GXVoid GXSoundEmitter::SetOwnVolume ( GXFloat ownVolumeLevel )
{
    ownVolume = ownVolumeLevel;
    GXAlSourcef ( source, AL_GAIN, ownVolume * channelVolume );
}

GXVoid GXSoundEmitter::SetChannelVolume ( GXFloat channelVolumeLevel )
{
    channelVolume = channelVolumeLevel;
    GXAlSourcef ( source, AL_GAIN, ownVolume * channelVolume );
}

GXVoid GXSoundEmitter::SetRange ( GXFloat min, GXFloat max )
{
    GXAlSourcef ( source, AL_REFERENCE_DISTANCE, min );
    GXAlSourcef ( source, AL_MAX_DISTANCE, max );
}

GXVoid GXSoundEmitter::ChangeSoundTrack ( GXSoundTrack* trackObject, GXBool isTrackLooped, GXBool isTrackStreamed, GXBool isRelative )
{
    if ( !trackObject )
        GXWarningBox ( L"GXSoundEmitter::Error - track равен 0!" );

    gx_sound_mixer_SmartLock->AcquireExclusive ();

    Stop ();

    if ( streamer )
    {
        GXAlSourcei ( source, AL_BUFFER, 0 );

        GXAlSourceUnqueueBuffers ( source, 1, streamBuffers );
        GXAlSourceUnqueueBuffers ( source, 1, streamBuffers + 1u );
        GXAlDeleteBuffers ( 2, streamBuffers );

        GXSafeDelete ( streamer );
    }
    else
    {
        GXAlSourcei ( source, AL_BUFFER, 0 );
    }

    track->Release ();
    track = trackObject;
    track->AddReference ();

    looped = isTrackLooped;

    if ( isTrackStreamed )
    {
        streamer = track->GetStreamer ();
        GXAlGenBuffers ( 2, streamBuffers );

        streamer->FillBuffer ( streamBuffers[ 0u ], looped );
        streamer->FillBuffer ( streamBuffers[ 1u ], looped );
        GXAlSourceQueueBuffers ( source, 2, streamBuffers );
    }
    else
    {
        streamer = nullptr;
        GXAlSourcei ( source, AL_LOOPING, looped ? AL_TRUE : AL_FALSE );

        ALuint bfr = track->GetBuffer ();
        GXAlSourcei ( source, AL_BUFFER, static_cast<ALint> ( bfr ) );
    }

    GXAlSourcei ( source, AL_SOURCE_RELATIVE, isRelative ? AL_TRUE : AL_FALSE );

    forceUpdate = GX_TRUE;
    stopped = GX_TRUE;
    finished = GX_FALSE;

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXSoundTrack* GXSoundEmitter::GetSoundTrack ()
{
    return track;
}

GXVoid GXSoundEmitter::Play ()
{
    GXAlSourcePlay ( source );

    if ( stopped ) return;

    stopped = GX_FALSE;

    if ( !streamer ) return;

    Rewind ();
}

GXVoid GXSoundEmitter::Stop ()
{
    GXAlSourceStop ( source );
    stopped = GX_TRUE;
}

GXVoid GXSoundEmitter::Pause ()
{
    GXAlSourcePause ( source );
}

GXVoid GXSoundEmitter::Rewind ()
{
    if ( !streamer )
    {
        GXAlSourceRewind ( source );
        return;
    }

    gx_sound_mixer_SmartLock->AcquireExclusive ();

    GXAlSourceStop ( source );
    streamer->Reset ();
    forceUpdate = GX_TRUE;
    Update ();
    forceUpdate = GX_FALSE;
    GXAlSourcePlay ( source );

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXVoid GXSoundEmitter::Update ()
{
    if ( !streamer ) return;

    GXInt temp;
    GXAlGetSourcei ( source, AL_BUFFERS_PROCESSED, &temp );

    while ( temp || forceUpdate )
    {
        GXAlSourceUnqueueBuffers ( source, 1, streamBuffers );
        finished = !streamer->FillBuffer ( streamBuffers[ 0u ], looped );

        if ( !finished )
            GXAlSourceQueueBuffers ( source, 1, streamBuffers );

        ALuint t = streamBuffers[ 1u ];
        streamBuffers[ 1u ] = streamBuffers[ 0u ];
        streamBuffers[ 0u ] = t;

        GXAlGetSourcei ( source, AL_BUFFERS_PROCESSED, &temp );

        if ( !forceUpdate ) continue;

        forceUpdate = GX_FALSE;
    }
}
