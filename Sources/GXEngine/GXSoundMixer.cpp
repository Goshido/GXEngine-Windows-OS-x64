// version 1.11

#include <GXEngine/GXSoundMixer.h>
#include <GXCommon/GXSmartLock.h>


#define DEFAULT_MASTER_VOLUME       1.0f
#define IDLE_DELAY                  10u

//---------------------------------------------------------------------------------------------------------------------

extern GXSmartLock* gx_sound_mixer_SmartLock;

GXBool              GXSoundMixer::loopFlag = GX_TRUE;
GXSoundChannel*     GXSoundMixer::channels = nullptr;
GXSoundMixer*       GXSoundMixer::instance = nullptr;

GXSoundMixer::~GXSoundMixer ()
{
    delete gx_sound_mixer_SmartLock;
    instance = nullptr;
}

GXVoid GXSoundMixer::SetListenerVelocity ( const GXVec3 &velocity )
{
    GXAlListenerfv ( AL_VELOCITY, velocity.data );
}

GXVoid GXSoundMixer::SetListenerVelocity ( GXFloat x, GXFloat y, GXFloat z )
{
    GXAlListener3f ( AL_VELOCITY, x, y, z );
}

GXVoid GXSoundMixer::SetListenerLocation ( const GXVec3 &location )
{
    GXAlListenerfv ( AL_POSITION, location.data );
}

GXVoid GXSoundMixer::SetListenerLocation ( GXFloat x, GXFloat y, GXFloat z )
{
    GXAlListener3f ( AL_POSITION, x, y, z );
}

GXVoid GXSoundMixer::SetListenerRotation ( const GXMat4 &rotation )
{
    GXFloat orientation[ 6u ];
    
    GXVec3 tmp;
    rotation.GetZ ( tmp );

    memcpy ( orientation, &tmp, sizeof ( GXVec3 ) );

    rotation.GetY ( tmp );
    memcpy ( orientation + 3u, &tmp, sizeof ( GXVec3 ) );

    orientation[ 0u ] = -orientation[ 0u ];
    orientation[ 1u ] = -orientation[ 1u ];
    orientation[ 2u ] = -orientation[ 2u ];

    GXAlListenerfv ( AL_ORIENTATION, orientation );
}

GXVoid GXSoundMixer::SetListenerRotation ( const GXEuler &rotation )
{
    GXMat4 orientation;
    orientation.RotationXYZ ( rotation.pitchRadians, rotation.yawRadians, rotation.rollRadians );

    SetListenerRotation ( orientation );
}

GXVoid GXSoundMixer::SetListenerRotation ( GXFloat pitchRadians, GXFloat yawRadians, GXFloat rollRadians )
{
    GXMat4 orientation;
    orientation.RotationXYZ ( pitchRadians, yawRadians, rollRadians );

    SetListenerRotation ( orientation );
}

GXVoid GXSoundMixer::AddChannel ( GXSoundChannel* channel )
{
    if ( !channel )
        GXWarningBox ( L"GXSoundMixer::AddChannel::Error - попытка добавить звуковой канал по нулевому указателю!" );

    channel->prev = nullptr;
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
    {
        GX_BIND_MEMORY_INSPECTOR_CLASS_NAME ( "GXSoundMixer" );
        instance = new GXSoundMixer ();
    }

    return *instance;
}

GXSoundMixer::GXSoundMixer ()
    GX_MEMORY_INSPECTOR_CONSTRUCTOR_NOT_LAST ( "GXSoundMixer" )
    thread ( &Update, nullptr ),
    masterVolume ( DEFAULT_MASTER_VOLUME )
{
    gx_sound_mixer_SmartLock = new GXSmartLock ();
}

GXUPointer GXTHREADCALL GXSoundMixer::Update ( GXVoid* /*args*/, GXThread &thread )
{
    while ( loopFlag )
    {
        gx_sound_mixer_SmartLock->AcquireShared ();

        for ( GXSoundChannel* c = channels; c; c = c->next )
            c->Update ();

        gx_sound_mixer_SmartLock->ReleaseShared ();

        thread.Sleep ( IDLE_DELAY );
    }

    return 0u;
}
