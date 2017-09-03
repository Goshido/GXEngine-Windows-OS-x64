//version 1.5

#include <GXEngine/GXSoundMixer.h>


#define GX_SOUND_MIXER_DELAY	10

extern GXMutex* gx_sound_mixer_Mutex;


GXBool				GXSoundMixer::loopFlag = GX_TRUE;
GXSoundChannel*		GXSoundMixer::channels = 0;

GXSoundMixer* GXSoundMixer::instance = nullptr;

GXSoundMixer::~GXSoundMixer ()
{
	delete gx_sound_mixer_Mutex;
	instance = nullptr;
}

GXVoid GXSoundMixer::SetListenerVelocity ( const GXVec3 &velocity )
{
	GXAlListenerfv ( AL_VELOCITY, velocity.arr );
}

GXVoid GXSoundMixer::SetListenerVelocity ( GXFloat x, GXFloat y, GXFloat z )
{
	GXAlListener3f ( AL_VELOCITY, x, y, z );
}

GXVoid GXSoundMixer::SetListenerLocation ( const GXVec3 &location )
{
	GXAlListenerfv ( AL_POSITION, location.arr );
}

GXVoid GXSoundMixer::SetListenerLocation ( GXFloat x, GXFloat y, GXFloat z )
{
	GXAlListener3f ( AL_POSITION, x, y, z );
}

GXVoid GXSoundMixer::SetListenerRotation ( const GXMat4 &rotation )
{
	GXFloat orientation[ 6 ];
	
	GXVec3 tmp;
	rotation.GetZ ( tmp );

	memcpy ( orientation, &tmp, sizeof ( GXVec3 ) );

	rotation.GetY ( tmp );
	memcpy ( orientation + 3, &tmp, sizeof ( GXVec3 ) );

	orientation[ 0 ] = -orientation[ 0 ];
	orientation[ 1 ] = -orientation[ 1 ];
	orientation[ 2 ] = -orientation[ 2 ];

	GXAlListenerfv ( AL_ORIENTATION, orientation );
}

GXVoid GXSoundMixer::SetListenerRotation ( const GXVec3 &rotation )
{
	GXMat4 orientation;
	GXSetMat4RotationXYZ ( orientation, rotation.pitch_rad, rotation.yaw_rad, rotation.roll_rad );

	SetListenerRotation ( orientation );
}

GXVoid GXSoundMixer::SetListenerRotation ( GXFloat pitch_rad, GXFloat yaw_rad, GXFloat roll_rad )
{
	GXMat4 orientation;
	GXSetMat4RotationXYZ ( orientation, pitch_rad, yaw_rad, roll_rad );

	SetListenerRotation ( orientation );
}

GXVoid GXSoundMixer::AddChannel ( GXSoundChannel* channel )
{
	if ( !channel ) GXDebugBox ( L"GXSoundMixer::AddChannel::Error - ������� �������� �������� ����� �� �������� ���������!" );

	channel->prev = 0;
	channel->next = channels;
	if ( channel->next ) channel->next->prev = channel;
	channels = channel;

	channel->top = &channels;
}

GXVoid GXSoundMixer::Start ()
{
	thread.Start ();
}

GXBool GXSoundMixer::Shutdown ()
{
	loopFlag = GX_FALSE;
	thread.Join ();

	return GX_TRUE;
}

GXVoid GXSoundMixer::SetMasterVolume ( GXFloat masterVolumeLevel )
{
	masterVolume = masterVolumeLevel;
	GXAlListenerf ( AL_GAIN, masterVolume );	
}

GXSoundMixer& GXCALL GXSoundMixer::GetInstance ()
{
	if ( !instance )
		instance = new GXSoundMixer ();

	return *instance;
}

GXSoundMixer::GXSoundMixer () :
thread ( &Update, nullptr )
{
	instance = this;

	loopFlag = GX_TRUE;

	masterVolume = 1.0f;

	channels = nullptr;

	gx_sound_mixer_Mutex = new GXMutex (); 
}

GXUPointer GXTHREADCALL GXSoundMixer::Update ( GXVoid* /*args*/, GXThread& /*thread*/ )
{
	while ( loopFlag )
	{
		gx_sound_mixer_Mutex->Lock ();

		for ( GXSoundChannel* c = channels; c; c = c->next )
			c->Update ();

		gx_sound_mixer_Mutex->Release ();

		Sleep ( GX_SOUND_MIXER_DELAY );
	}

	return 0;
}
