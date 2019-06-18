// version 1.10

#include <GXEngine/GXSoundEmitter.h>
#include <GXCommon/GXSmartLock.h>


GXSmartLock* gx_sound_mixer_SmartLock = nullptr;

GXSoundEmitter::GXSoundEmitter ( GXSoundTrack* track, GXBool isTrackLooped, GXBool isTrackStreamed, GXBool isRelative )
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_SINGLE ( "GXSoundEmitter" )
{
    if ( !track )
        GXWarningBox ( L"GXSoundEmitter::Error - track равен nullptr!" );

    gx_sound_mixer_SmartLock->AcquireExclusive ();

    _top = nullptr;
    _next = _previous = nullptr;

    _stopped = GX_TRUE;
    _finished = GX_FALSE;

    _forceUpdate = GX_FALSE;

    _source = 0u;
    _streamBuffers[ 0u ] = _streamBuffers[ 1u ] = 0u;


    _track = track;
    _looped = isTrackLooped;

    _track->AddReference ();

    GXAlGenSources ( 1, &_source );

    if ( isTrackStreamed )
    {
        _streamer = _track->GetStreamer ();
        GXAlGenBuffers ( 2, _streamBuffers );

        _streamer->FillBuffer ( _streamBuffers[ 0u ], _looped );
        _streamer->FillBuffer ( _streamBuffers[ 1u ], _looped );
        GXAlSourceQueueBuffers ( _source, 2, _streamBuffers );
    }
    else
    {
        _streamer = nullptr;
        GXAlSourcei ( _source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE );

        ALuint bfr = _track->GetBuffer ();
        GXAlSourcei ( _source, AL_BUFFER, static_cast<ALint> ( bfr ) );
    }

    _ownVolume = 1.0f;
    SetChannelVolume ( 1.0f );

    GXAlSourcei ( _source, AL_SOURCE_RELATIVE, isRelative ? AL_TRUE : AL_FALSE );

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXSoundEmitter::~GXSoundEmitter ()
{
    gx_sound_mixer_SmartLock->AcquireExclusive ();

    Stop ();
    GXAlDeleteSources ( 1, &_source );
    
    if ( _streamer )
    {
        GXAlSourceUnqueueBuffers ( _source, 1, _streamBuffers );
        GXAlSourceUnqueueBuffers ( _source, 1, _streamBuffers + 1u );
        GXAlDeleteBuffers ( 2, _streamBuffers );

        delete _streamer;
    }

    _track->Release ();

    if ( _top )
    {
        if ( _next )
            _next->_previous = _previous;

        if ( _previous )
        {
            _previous->_next = _next;
        }
        else
        {
            *_top = _next;
        }
    }

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXVoid GXSoundEmitter::SetVelocity ( const GXVec3 &velocity )
{
    GXAlSourcefv ( _source, AL_VELOCITY, velocity.data );
}

GXVoid GXSoundEmitter::SetVelocity ( GXFloat x, GXFloat y, GXFloat z )
{
    GXAlSource3f ( _source, AL_VELOCITY, x, y, z );
}

GXVoid GXSoundEmitter::SetLocation ( const GXVec3 &location )
{
    GXAlSourcefv ( _source, AL_POSITION, location.data );
}

GXVoid GXSoundEmitter::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    GXAlSource3f ( _source, AL_POSITION, x, y, z );
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

    GXAlSourcefv ( _source, AL_ORIENTATION, orientation );
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
    _ownVolume = ownVolumeLevel;
    GXAlSourcef ( _source, AL_GAIN, _ownVolume * _channelVolume );
}

GXVoid GXSoundEmitter::SetChannelVolume ( GXFloat channelVolumeLevel )
{
    _channelVolume = channelVolumeLevel;
    GXAlSourcef ( _source, AL_GAIN, _ownVolume * _channelVolume );
}

GXVoid GXSoundEmitter::SetRange ( GXFloat min, GXFloat max )
{
    GXAlSourcef ( _source, AL_REFERENCE_DISTANCE, min );
    GXAlSourcef ( _source, AL_MAX_DISTANCE, max );
}

GXVoid GXSoundEmitter::ChangeSoundTrack ( GXSoundTrack* trackObject, GXBool isTrackLooped, GXBool isTrackStreamed, GXBool isRelative )
{
    if ( !trackObject )
        GXWarningBox ( L"GXSoundEmitter::Error - track равен 0!" );

    gx_sound_mixer_SmartLock->AcquireExclusive ();

    Stop ();

    if ( _streamer )
    {
        GXAlSourcei ( _source, AL_BUFFER, 0 );

        GXAlSourceUnqueueBuffers ( _source, 1, _streamBuffers );
        GXAlSourceUnqueueBuffers ( _source, 1, _streamBuffers + 1u );
        GXAlDeleteBuffers ( 2, _streamBuffers );

        GXSafeDelete ( _streamer );
    }
    else
    {
        GXAlSourcei ( _source, AL_BUFFER, 0 );
    }

    _track->Release ();
    _track = trackObject;
    _track->AddReference ();

    _looped = isTrackLooped;

    if ( isTrackStreamed )
    {
        _streamer = _track->GetStreamer ();
        GXAlGenBuffers ( 2, _streamBuffers );

        _streamer->FillBuffer ( _streamBuffers[ 0u ], _looped );
        _streamer->FillBuffer ( _streamBuffers[ 1u ], _looped );
        GXAlSourceQueueBuffers ( _source, 2, _streamBuffers );
    }
    else
    {
        _streamer = nullptr;
        GXAlSourcei ( _source, AL_LOOPING, _looped ? AL_TRUE : AL_FALSE );

        ALuint bfr = _track->GetBuffer ();
        GXAlSourcei ( _source, AL_BUFFER, static_cast<ALint> ( bfr ) );
    }

    GXAlSourcei ( _source, AL_SOURCE_RELATIVE, isRelative ? AL_TRUE : AL_FALSE );

    _forceUpdate = GX_TRUE;
    _stopped = GX_TRUE;
    _finished = GX_FALSE;

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXSoundTrack* GXSoundEmitter::GetSoundTrack ()
{
    return _track;
}

GXVoid GXSoundEmitter::Play ()
{
    GXAlSourcePlay ( _source );

    if ( _stopped ) return;

    _stopped = GX_FALSE;

    if ( !_streamer ) return;

    Rewind ();
}

GXVoid GXSoundEmitter::Stop ()
{
    GXAlSourceStop ( _source );
    _stopped = GX_TRUE;
}

GXVoid GXSoundEmitter::Pause ()
{
    GXAlSourcePause ( _source );
}

GXVoid GXSoundEmitter::Rewind ()
{
    if ( !_streamer )
    {
        GXAlSourceRewind ( _source );
        return;
    }

    gx_sound_mixer_SmartLock->AcquireExclusive ();

    GXAlSourceStop ( _source );
    _streamer->Reset ();
    _forceUpdate = GX_TRUE;
    Update ();
    _forceUpdate = GX_FALSE;
    GXAlSourcePlay ( _source );

    gx_sound_mixer_SmartLock->ReleaseExclusive ();
}

GXVoid GXSoundEmitter::Update ()
{
    if ( !_streamer ) return;

    GXInt temp;
    GXAlGetSourcei ( _source, AL_BUFFERS_PROCESSED, &temp );

    while ( temp || _forceUpdate )
    {
        GXAlSourceUnqueueBuffers ( _source, 1, _streamBuffers );
        _finished = !_streamer->FillBuffer ( _streamBuffers[ 0u ], _looped );

        if ( !_finished )
            GXAlSourceQueueBuffers ( _source, 1, _streamBuffers );

        ALuint t = _streamBuffers[ 1u ];
        _streamBuffers[ 1u ] = _streamBuffers[ 0u ];
        _streamBuffers[ 0u ] = t;

        GXAlGetSourcei ( _source, AL_BUFFERS_PROCESSED, &temp );

        if ( !_forceUpdate ) continue;

        _forceUpdate = GX_FALSE;
    }
}
