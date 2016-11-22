//version 1.4

#include <GXEngine/GXSoundEmitter.h>
#include <GXCommon/GXMutex.h>


GXMutex* gx_sound_mixer_Mutex = 0;


GXSoundEmitter::GXSoundEmitter ( GXSoundTrack* track, GXBool looped, GXBool streamed, GXBool isRelative )
{
	if ( !track ) GXDebugBox ( L"GXSoundEmitter::Error - track равен 0!" );

	gx_sound_mixer_Mutex->Lock ();

	forceUpdate = GX_FALSE;
	stopped = GX_TRUE;
	finished = GX_FALSE;

	source = 0;
	streamBuffers[ 0 ] = streamBuffers[ 1 ] = 0;

	top = 0;
	next = prev = 0;

	this->track = track;
	this->looped = looped;

	this->track->AddRef ();

	GXAlGenSources ( 1, &source );

	if ( streamed ) 
	{
		streamer = this->track->GetStreamer ();
		GXAlGenBuffers ( 2, streamBuffers );

		streamer->FillBuffer ( streamBuffers[ 0 ], this->looped );
		streamer->FillBuffer ( streamBuffers[ 1 ], this->looped );
		GXAlSourceQueueBuffers ( source, 2, streamBuffers );
	}
	else
	{
		streamer = 0;
		GXAlSourcei ( source, AL_LOOPING, this->looped ? AL_TRUE : AL_FALSE );

		ALuint bfr = this->track->GetBuffer ();
		GXAlSourcei ( source, AL_BUFFER, bfr );
	}

	ownVolume = 1.0f;
	SetChannelVolume ( 1.0f );

	GXAlSourcei ( source, AL_SOURCE_RELATIVE, isRelative ? AL_TRUE : AL_FALSE );

	gx_sound_mixer_Mutex->Release ();
}

GXSoundEmitter::~GXSoundEmitter ()
{
	gx_sound_mixer_Mutex->Lock ();

	Stop ();
	GXAlDeleteSources ( 1, &source );
	
	if ( streamer )
	{
		GXAlSourceUnqueueBuffers ( source, 1, streamBuffers );
		GXAlSourceUnqueueBuffers ( source, 1, streamBuffers + 1 );
		GXAlDeleteBuffers ( 2, streamBuffers );

		delete streamer;
	}

	track->Release ();

	if ( top )
	{
		if ( next ) next->prev = prev;
		if ( prev ) prev->next = next;
		else *top = next;
	}

	gx_sound_mixer_Mutex->Release ();
}

GXVoid GXSoundEmitter::SetVelocity ( const GXVec3 &velocity )
{
	GXAlSourcefv ( source, AL_VELOCITY, velocity.v );
}

GXVoid GXSoundEmitter::SetVelocity ( GXFloat x, GXFloat y, GXFloat z )
{
	GXAlSource3f ( source, AL_VELOCITY, x, y, z );
}

GXVoid GXSoundEmitter::SetLocation ( const GXVec3 &location )
{
	GXAlSourcefv ( source, AL_POSITION, location.v );
}

GXVoid GXSoundEmitter::SetLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXAlSource3f ( source, AL_POSITION, x, y, z );
}

GXVoid GXSoundEmitter::SetRotation ( const GXMat4 &rotation )
{
	GXFloat orientation[ 6 ];
	
	memcpy ( orientation, rotation.zv.v, sizeof ( GXVec3 ) );
	memcpy ( orientation + 3, rotation.yv.v, sizeof ( GXVec3 ) );

	orientation[ 0 ] = -orientation[ 0 ];
	orientation[ 1 ] = -orientation[ 1 ];
	orientation[ 2 ] = -orientation[ 2 ];

	GXAlSourcefv ( source, AL_ORIENTATION, orientation );
}

GXVoid GXSoundEmitter::SetRotation ( const GXVec3 &rotation )
{
	GXMat4 orientation;
	GXSetMat4RotationXYZ ( orientation, rotation.pitch_rad, rotation.yaw_rad, rotation.roll_rad );

	SetRotation ( orientation );
}

GXVoid GXSoundEmitter::SetRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXMat4 orientation;
	GXSetMat4RotationXYZ ( orientation, pitch_rad, yaw_rad, roll_rad );

	SetRotation ( orientation );
}

GXVoid GXSoundEmitter::SetOwnVolume ( GXFloat ownVolume )
{
	this->ownVolume = ownVolume;
	GXAlSourcef ( source, AL_GAIN, ownVolume * channelVolume );
}

GXVoid GXSoundEmitter::SetChannelVolume ( GXFloat channelVolume )
{
	this->channelVolume = channelVolume;
	GXAlSourcef ( source, AL_GAIN, ownVolume * channelVolume );
}

GXVoid GXSoundEmitter::SetRange ( GXFloat min, GXFloat max )
{
	GXAlSourcef ( source, AL_REFERENCE_DISTANCE, min );
	GXAlSourcef ( source, AL_MAX_DISTANCE, max );
}

GXVoid GXSoundEmitter::ChangeSoundTrack ( GXSoundTrack* track, GXBool looped, GXBool streamed, GXBool isRelative )
{
	if ( !track ) GXDebugBox ( L"GXSoundEmitter::Error - track равен 0!" );

	gx_sound_mixer_Mutex->Lock ();

	Stop ();

	if ( streamer )
	{
		GXAlSourcei ( source, AL_BUFFER, 0 );

		GXAlSourceUnqueueBuffers ( source, 1, streamBuffers );
		GXAlSourceUnqueueBuffers ( source, 1, streamBuffers + 1 );
		GXAlDeleteBuffers ( 2, streamBuffers );

		GXSafeDelete ( streamer );
	}
	else 
		GXAlSourcei ( source, AL_BUFFER, 0 );

	this->track->Release ();
	this->track = track;
	this->track->AddRef ();

	this->looped = looped;

	if ( streamed )
	{
		streamer = this->track->GetStreamer ();
		GXAlGenBuffers ( 2, streamBuffers );

		streamer->FillBuffer ( streamBuffers[ 0 ], this->looped );
		streamer->FillBuffer ( streamBuffers[ 1 ], this->looped );
		GXAlSourceQueueBuffers ( source, 2, streamBuffers );
	}
	else
	{
		streamer = 0;
		GXAlSourcei ( source, AL_LOOPING, this->looped ? AL_TRUE : AL_FALSE );

		ALuint bfr = this->track->GetBuffer ();
		GXAlSourcei ( source, AL_BUFFER, bfr );
	}

	GXAlSourcei ( source, AL_SOURCE_RELATIVE, isRelative ? AL_TRUE : AL_FALSE );

	forceUpdate = GX_TRUE;
	stopped = GX_TRUE;
	finished = GX_FALSE;

	gx_sound_mixer_Mutex->Release ();
}

GXSoundTrack* GXSoundEmitter::GetSoundTrack ()
{
	return track;
}

GXVoid GXSoundEmitter::Play ()
{
	GXAlSourcePlay ( source );
	if ( !stopped )
	{
		stopped = GX_FALSE;
		if ( streamer ) 
			Rewind ();
	}
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
	if ( streamer )
	{
		gx_sound_mixer_Mutex->Lock ();

		GXAlSourceStop ( source );
		streamer->Reset ();
		forceUpdate = GX_TRUE;
		Update ();
		forceUpdate = GX_FALSE;
		GXAlSourcePlay ( source );

		gx_sound_mixer_Mutex->Release ();
	}
	else
		GXAlSourceRewind ( source );
}

GXVoid GXSoundEmitter::Update ()
{
	if ( !streamer ) return;

	GXInt temp;
	GXAlGetSourcei ( source, AL_BUFFERS_PROCESSED, &temp );

	while ( temp || forceUpdate )
	{
		GXAlSourceUnqueueBuffers ( source, 1, streamBuffers );
		finished = !streamer->FillBuffer ( streamBuffers[ 0 ], looped );

		if ( !finished )
			GXAlSourceQueueBuffers ( source, 1, streamBuffers );

		ALuint t = streamBuffers[ 1 ];
		streamBuffers[ 1 ] = streamBuffers[ 0 ];
		streamBuffers[ 0 ] = t;

		GXAlGetSourcei ( source, AL_BUFFERS_PROCESSED, &temp );

		if ( forceUpdate ) forceUpdate = GX_FALSE;
	}
}